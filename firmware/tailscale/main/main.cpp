#include <Arduino.h>
#if ARDUINO_USB_CDC_ON_BOOT && !ARDUINO_USB_MODE
#include "USB.h"
#endif
#include "controller.h"

#include <string.h>
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "esp_timer.h"
#include <stdarg.h>
#include "esp_log.h"

// MicroLink pins ml_coord (priority 5) and ml_wg_mgr (priority 7) to core 1 and
// logs through the blocking UART0 console at 115200 baud. At its old priority 2
// the control loop was kept off core 1 for longer than its 250 ms watchdog while
// the peer map was logged, and the chip rebooted. 10 keeps the loop above both,
// and below lwIP (18), Arduino events (19) and TinyUSB (24), which only run briefly.
constexpr UBaseType_t kControllerPriority = 10;

// ---- Reboot diagnostics -------------------------------------------------------
// The only console of this build is the TinyUSB CDC port, which disappears with
// every chip reset, so panic dumps and task-watchdog reports are never visible.
// Keep a small record in RTC memory (survives software, panic and watchdog
// resets) and print it once each time a terminal asserts DTR on the port.
namespace diag {
constexpr uint32_t kMagic = 0x4954444Au;  // "ITDJ"
constexpr int kCanaryLow = 0;   // priority 3: what the control loop saw at its old priority 2
constexpr int kCanaryCtrl = 1;  // kControllerPriority + 1: what the control loop still competes with
struct Record {
  uint32_t magic;
  uint32_t boots;
  uint32_t uptimeMs;         // last known uptime of that run
  uint32_t gapMaxMs[2];      // longest scheduling gap seen by each core-1 canary
  uint32_t gapAtMs[2];       // uptime when each gapMaxMs was recorded
  uint32_t minFreeInternal;  // low-water mark of the internal heap
  char tailnet[32];          // last monitor state, e.g. "connecting"
  char twdt[160];            // task watchdog report, if it fired
  char panic[160];           // panic reason, if any
};
RTC_NOINIT_ATTR Record live;  // written by this run
static Record previous;       // what the previous run left behind
static esp_reset_reason_t bootReason = ESP_RST_UNKNOWN;

// ---- Log capture ---------------------------------------------------------------
// ESP_LOG output went to UART0, whose pins this board uses as D6/D7, and every
// character blocked the logging task for 87 us at 115200 baud (the MicroLink
// peer-map dump alone exceeded the 250 ms watchdog budget). Capture it in a ring
// buffer instead and stream it to the USB console while a terminal is connected;
// the newest 32 KB are kept until then.
constexpr size_t kLogRing = 32 * 1024;
static char* ring = nullptr;
static size_t ringHead = 0, ringTail = 0;
static uint32_t ringDropped = 0;  // bytes overwritten before a host read them
static portMUX_TYPE ringMux = portMUX_INITIALIZER_UNLOCKED;
static int logVprintf(const char* fmt, va_list ap) {
  char buf[256];
  int n = vsnprintf(buf, sizeof buf, fmt, ap);
  if (n <= 0) return n;
  size_t len = size_t(n) < sizeof buf ? size_t(n) : sizeof buf - 1;
  if (len == sizeof buf - 1) buf[len - 1] = '\n';  // keep lines separate when truncated
  portENTER_CRITICAL_SAFE(&ringMux);
  for (size_t i = 0; i < len; i++) {
    ring[ringHead] = buf[i];
    ringHead = (ringHead + 1) % kLogRing;
    if (ringHead == ringTail) { ringTail = (ringTail + 1) % kLogRing; ringDropped++; }
  }
  portEXIT_CRITICAL_SAFE(&ringMux);
  return n;
}
// One chunk per call so the canary loop keeps its 1 ms cadence.
static void drainLog() {
  int cap = Serial.availableForWrite();
  if (cap <= 0) return;
  uint8_t chunk[64];
  size_t n = 0;
  portENTER_CRITICAL_SAFE(&ringMux);
  while (ringTail != ringHead && n < sizeof chunk && int(n) < cap) {
    chunk[n++] = uint8_t(ring[ringTail]);
    ringTail = (ringTail + 1) % kLogRing;
  }
  portEXIT_CRITICAL_SAFE(&ringMux);
  if (n) Serial.write(chunk, n);
}

// Bounded append without libc; usable from the watchdog ISR and the panic path.
static void append(char* dst, size_t cap, const char* src) {
  size_t n = 0;
  while (n + 1 < cap && dst[n]) n++;
  while (src && *src && n + 1 < cap) dst[n++] = *src++;
  dst[n] = 0;
}
static void appendHex(char* dst, size_t cap, uintptr_t v) {
  char buf[11] = "0x";
  const char* hex = "0123456789abcdef";
  for (int i = 0; i < 8; i++) buf[2 + i] = hex[(v >> ((7 - i) * 4)) & 0xf];
  buf[10] = 0;
  append(dst, cap, buf);
}
static const char* reasonName(esp_reset_reason_t r) {
  switch (r) {
    case ESP_RST_POWERON: return "POWERON";
    case ESP_RST_EXT: return "EXT";
    case ESP_RST_SW: return "SW";
    case ESP_RST_PANIC: return "PANIC";
    case ESP_RST_INT_WDT: return "INT_WDT";
    case ESP_RST_TASK_WDT: return "TASK_WDT";
    case ESP_RST_WDT: return "WDT";
    case ESP_RST_DEEPSLEEP: return "DEEPSLEEP";
    case ESP_RST_BROWNOUT: return "BROWNOUT";
    case ESP_RST_SDIO: return "SDIO";
    case ESP_RST_USB: return "USB";
    case ESP_RST_JTAG: return "JTAG";
    case ESP_RST_EFUSE: return "EFUSE";
    case ESP_RST_PWR_GLITCH: return "PWR_GLITCH";
    case ESP_RST_CPU_LOCKUP: return "CPU_LOCKUP";
    default: return "UNKNOWN";
  }
}
static void onBoot() {
  bootReason = esp_reset_reason();
  if (live.magic != kMagic) {
    memset(&live, 0, sizeof live);
    live.magic = kMagic;
  }
  previous = live;
  live.boots++;
  live.uptimeMs = live.minFreeInternal = 0;
  for (int i = 0; i < 2; i++) live.gapMaxMs[i] = live.gapAtMs[i] = 0;
  live.tailnet[0] = live.twdt[0] = live.panic[0] = 0;
}
// Chunked write that never blocks on the host; gives up after 2 s.
static void write(const char* s) {
  size_t n = strlen(s), off = 0;
  uint32_t start = millis();
  while (off < n && Serial && uint32_t(millis() - start) < 2000) {
    int cap = Serial.availableForWrite();
    if (cap <= 0) { vTaskDelay(1); continue; }
    size_t chunk = n - off < size_t(cap) ? n - off : size_t(cap);
    size_t w = Serial.write(reinterpret_cast<const uint8_t*>(s) + off, chunk);
    off += w;
    if (!w) vTaskDelay(1);
  }
}
static void report() {
  char line[320];
  snprintf(line, sizeof line, "DIAG boot=%lu reset_reason=%s(%d) prev_uptime_ms=%lu prev_tailnet=%s\n",
           (unsigned long)live.boots, reasonName(bootReason), int(bootReason),
           (unsigned long)previous.uptimeMs, previous.tailnet[0] ? previous.tailnet : "-");
  write(line);
  snprintf(line, sizeof line,
           "DIAG prev_gap_lowprio_ms=%lu@%lu prev_gap_ctrl_ms=%lu@%lu prev_min_free_internal=%lu\n",
           (unsigned long)previous.gapMaxMs[kCanaryLow], (unsigned long)previous.gapAtMs[kCanaryLow],
           (unsigned long)previous.gapMaxMs[kCanaryCtrl], (unsigned long)previous.gapAtMs[kCanaryCtrl],
           (unsigned long)previous.minFreeInternal);
  write(line);
  if (previous.twdt[0]) { snprintf(line, sizeof line, "DIAG prev_twdt=%s\n", previous.twdt); write(line); }
  if (previous.panic[0]) { snprintf(line, sizeof line, "DIAG prev_panic=%s\n", previous.panic); write(line); }
  snprintf(line, sizeof line,
           "DIAG now uptime_ms=%lu ctrl_prio=%u gap_lowprio_ms=%lu@%lu gap_ctrl_ms=%lu@%lu "
           "min_free_internal=%lu free_internal=%lu log_dropped=%lu\n",
           (unsigned long)live.uptimeMs, unsigned(kControllerPriority),
           (unsigned long)live.gapMaxMs[kCanaryLow], (unsigned long)live.gapAtMs[kCanaryLow],
           (unsigned long)live.gapMaxMs[kCanaryCtrl], (unsigned long)live.gapAtMs[kCanaryCtrl],
           (unsigned long)live.minFreeInternal,
           (unsigned long)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
           (unsigned long)ringDropped);
  write(line);
  write("DIAG log follows (ESP-IDF log, newest 32 KB kept while no terminal was attached)\n");
}
// Core-1 canaries. Each sleeps one tick and records the longest time it was kept
// off the CPU. kCanaryLow runs at priority 3, below MicroLink's core-1 tasks
// (ml_coord 5, ml_wg_mgr 7): its gaps show what the control loop suffered at its
// old priority 2. kCanaryCtrl runs at kControllerPriority + 1: its gaps are the
// preemption the control loop still sees (lwIP, TinyUSB, flash-cache stalls).
// Only the low canary samples state and prints the report.
static void task(void* arg) {
  const int slot = int(reinterpret_cast<intptr_t>(arg));
  int64_t last = esp_timer_get_time();
  uint32_t lastSample = 0, connectedAt = 0;
  bool announced = false;
  for (;;) {
    vTaskDelay(1);
    int64_t now = esp_timer_get_time();
    uint32_t gap = uint32_t((now - last) / 1000);
    last = now;
    uint32_t ms = uint32_t(now / 1000);
    if (gap > live.gapMaxMs[slot]) { live.gapMaxMs[slot] = gap; live.gapAtMs[slot] = ms; }
    if (slot != kCanaryLow) continue;
    if (uint32_t(ms - lastSample) >= 100) {
      lastSample = ms;
      live.uptimeMs = ms;
      live.minFreeInternal = heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
      char text[120];
      monitor::status(text, sizeof text);  // "tailnet=<state> monitor_only=1\n"
      const char* p = strchr(text, '=');
      p = p ? p + 1 : text;
      size_t i = 0;
      while (p[i] && p[i] != ' ' && p[i] != '\n' && i + 1 < sizeof live.tailnet) { live.tailnet[i] = p[i]; i++; }
      live.tailnet[i] = 0;
    }
    if (Serial) {
      if (connectedAt == 0) connectedAt = ms ? ms : 1;
      // Let the host finish opening the port before the report, then stream the log.
      if (!announced && uint32_t(ms - connectedAt) >= 300) {
        announced = true;
        report();
        last = esp_timer_get_time();
      }
      if (announced) drainLog();
    } else {
      announced = false;
      connectedAt = 0;
    }
  }
}
}  // namespace diag

// Task watchdog: record which entries starved before ESP-IDF aborts.
extern "C" void esp_task_wdt_isr_user_handler(void) {
  diag::live.twdt[0] = 0;
  int cpus = 0;
  esp_task_wdt_print_triggered_tasks(
    [](void* opaque, const char* msg) { diag::append(static_cast<char*>(opaque), sizeof diag::live.twdt, msg); },
    diag::live.twdt, &cpus);
  diag::live.uptimeMs = uint32_t(esp_timer_get_time() / 1000);
}

// Panic: the Arduino core wraps esp_panic_handler (the link flag in CMakeLists.txt
// activates it) and hands us core, reason, PC and a backtrace before ESP-IDF
// prints its own dump. Record the first frames; decode later with
// xtensa-esp32s3-elf-addr2line -e build/ito_tailscale_monitor.elf <addresses>.
static void onPanic(arduino_panic_info_t* info, void*) {
  char* out = diag::live.panic;
  out[0] = 0;
  diag::append(out, sizeof diag::live.panic, info->core ? "core1 " : "core0 ");
  diag::append(out, sizeof diag::live.panic, info->reason ? info->reason : "?");
  diag::append(out, sizeof diag::live.panic, " pc=");
  diag::appendHex(out, sizeof diag::live.panic, reinterpret_cast<uintptr_t>(info->pc));
  diag::append(out, sizeof diag::live.panic, " bt=");
  for (unsigned i = 0; i < info->backtrace_len && i < 8; i++) {
    diag::appendHex(out, sizeof diag::live.panic, info->backtrace[i]);
    diag::append(out, sizeof diag::live.panic, " ");
  }
  if (info->backtrace_corrupt) diag::append(out, sizeof diag::live.panic, "(corrupt)");
  diag::live.uptimeMs = uint32_t(esp_timer_get_time() / 1000);
}

static void controllerTask(void*) {
  setup();
  // Arduino-ESP32 3.x USBCDC::write() tests its timeout before sending the first
  // byte, so the zero timeout chosen in setup() silently drops every reply. 5 ms
  // keeps writes bounded (serialFlush never queues more than availableForWrite)
  // while letting the "wifi"/"status" answers reach the host.
  Serial.setTxTimeoutMs(5);
  for (;;) loop();
}

extern "C" void app_main() {
  diag::onBoot();
  set_arduino_panic_handler(onPanic, nullptr);
  diag::ring = static_cast<char*>(heap_caps_malloc(diag::kLogRing, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
  if (diag::ring) esp_log_set_vprintf(diag::logVprintf);
  // CONFIG_AUTOSTART_ARDUINO=n skips the Arduino core's own app_main, which is the
  // only place that starts the TinyUSB stack. Mirror its sequence so the native
  // USB CDC console (Serial, "status"/"wifi" commands) enumerates as on Arduino IDE.
#if ARDUINO_USB_CDC_ON_BOOT && !ARDUINO_USB_MODE
  Serial.begin();
  USB.begin();
#endif
  initArduino();
  // Keep the controller loop on core 1, above MicroLink's core-1 tasks (see
  // kControllerPriority); Wi-Fi, MQTT and the rest of MicroLink run on core 0.
  if (xTaskCreatePinnedToCore(controllerTask, "ito-control", 8192, nullptr,
                            kControllerPriority, nullptr, 1) != pdPASS) {
    // setup() never ran: explicitly keep all three power enables low.
    pinMode(D3, OUTPUT); digitalWrite(D3, LOW);
    pinMode(D6, OUTPUT); digitalWrite(D6, LOW);
    pinMode(D9, OUTPUT); digitalWrite(D9, LOW);
  }
  xTaskCreatePinnedToCore(diag::task, "ito-diag-lo", 4096,
                          reinterpret_cast<void*>(diag::kCanaryLow), 3, nullptr, 1);
  xTaskCreatePinnedToCore(diag::task, "ito-diag-hi", 2560,
                          reinterpret_cast<void*>(diag::kCanaryCtrl), kControllerPriority + 1, nullptr, 1);
}
