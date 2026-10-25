# ITO Tailscale monitor: notes for Claude Code

Handoff written 2026-10-25 after debugging on the owner's Windows PC. Read fully
before building, flashing or changing anything here.

## What this is

ESP-IDF 5.5.1 build (Arduino-ESP32 3.3.2 as a component) for the Seeed XIAO
ESP32-S3 with octal PSRAM. It runs the unchanged controller from
`../esp32s3/ito_prototype_esp32s3` in monitor-only mode on core 1, joins
Tailscale through MicroLink, and publishes telemetry over plain MQTT inside
WireGuard to the broker on the owner's tailnet, once per second. No MQTT
subscriptions, no power commands.

## Rules from the owner

- Telemetry only. Never enable heater/TEC paths, change `monitor::enabled`, the
  250 ms task watchdog in `platform.h`, the output locks, or anything in
  `../esp32s3/ito_prototype_esp32s3/*` without an explicit request; those files
  are safety-reviewed and shared with the Arduino IDE build.
- Flash or erase only when the user asks, with the XIAO off the PCB and powered
  from USB. Never `erase_flash` or wipe NVS as "recovery": MicroLink's node
  identity lives there.
- Do not change broker or server settings, tailnet policy, or move to ESP-IDF 6.x.
  Broker, ACL and dashboard work is the owner's, on the server side.
- Secrets: `main/tailnet_local.h` is gitignored and holds Wi-Fi, Tailscale and
  MQTT credentials. Never print, `cat`, `git diff` or commit it or the example
  header's contents; check them only with `cmp`, hashes, or scripts that print
  macro names and lengths. The tracked `tailnet_local.example.h` must keep
  placeholders. Build outputs (`build/`, any `.bin`/`.elf`/zip) embed the
  secrets: never commit, upload or paste them.

## Build and flash on macOS

```sh
. ~/esp/esp-idf/export.sh                 # ESP-IDF 5.5.1, not 6.x
python3 bootstrap.py                      # pinned vendors into vendor/ (refuses a dirty tree)
cp main/tailnet_local.example.h main/tailnet_local.h   # then the owner fills it in locally
idf.py set-target esp32s3 && idf.py build
```

Vendors pinned by `bootstrap.py`: arduino-esp32 3.3.2, esp32-arduino-lib-builder
plus its tinyusb, microlink 216da33, pubsubclient. `managed_components/`,
`dependencies.lock` and `sdkconfig` are generated and gitignored.

Flashing: hold BOOT, tap RESET, release BOOT. The ROM's USB-Serial/JTAG port
appears as `/dev/cu.usbmodem*`; then `idf.py -p /dev/cu.usbmodemXXXX flash`.
After reset the app enumerates as a different usbmodem device (Seeed TinyUSB CDC,
VID 2886 PID 0056). A chip reset by software or panic re-enumerates straight to
that CDC port; only a power-on or RESET press shows the ROM port first.

## Reading the console

`idf.py monitor` leaves DTR low and this firmware drops console output while no
terminal asserts DTR, so use pyserial's terminal (installed with ESP-IDF):

```sh
python3 -m serial.tools.miniterm /dev/cu.usbmodemXXXX 115200
```

Read-only commands: `wifi`, `status`, `pins`, `ambient`. Do not send `stop`,
`reset`, `heat` or `tec`. About 300 ms after connecting the firmware prints
`DIAG` lines, then `DIAG log follows` and the ESP-IDF log (MicroLink included);
the newest 32 KB are kept while no terminal is attached.

| DIAG field | Healthy | Problem |
|---|---|---|
| `reset_reason` after a minute of uptime | `USB`, `POWERON`, `EXT` | `TASK_WDT` or `PANIC`: it rebooted |
| `gap_ctrl_ms` | far below 250 | near 250: the control loop is being preempted again |
| `gap_lowprio_ms` | large values are expected | shows what priority 2 used to suffer |
| `prev_twdt` | absent | names the starved watchdog entry |
| `prev_panic` | absent | `pc=`/`bt=` addresses; decode with `xtensa-esp32s3-elf-addr2line -e build/ito_tailscale_monitor.elf <addr>` |

## How the code is arranged

- `main/main.cpp`: `app_main` starts TinyUSB (the Arduino core's own autostart is
  off), runs `initArduino()`, creates the control loop task `ito-control` on core 1
  at priority `kControllerPriority` (10), and two diagnostic canary tasks.
  MicroLink pins `ml_coord` (priority 5) and `ml_wg_mgr` (priority 7) to core 1;
  lwIP (18), Arduino network events (19) and TinyUSB `usbd` (24) float.
- Diagnostics in `main.cpp`: an `RTC_NOINIT_ATTR` record survives software, panic
  and watchdog resets. Task-watchdog culprits come from
  `esp_task_wdt_isr_user_handler`; panic PC and backtrace from
  `set_arduino_panic_handler`, which only works with
  `-Wl,--wrap=esp_panic_handler` in `main/CMakeLists.txt`. The Arduino core
  already defines `__wrap_esp_panic_handler`; defining another breaks the link.
  ESP-IDF log output is captured by `esp_log_set_vprintf` into a 32 KB PSRAM ring
  and streamed to the CDC port by the `ito-diag-lo` task.
- Arduino-ESP32 3.3.2 `USBCDC::write()` checks its timeout before the first byte,
  so the controller's `Serial.setTxTimeoutMs(0)` would drop every reply;
  `main.cpp` sets 5 ms right after `setup()`. `controller.h` still sets 0, which
  also affects the Arduino IDE build; left for the owner to decide.
- UART0 is not a console: its pins are the D6/D7 outputs. Panic text is only
  available through the RTC record.
- `CMakeLists.txt` has a block that registers MicroLink's `wireguard_lwip`
  component directly when the git symlink was checked out as a file (Windows).
  It is a no-op on macOS and Linux.

## Status on 2026-10-25

- Fixed: the board rebooted every 12 to 20 s. Cause: MicroLink's core-1 tasks
  outranked the control loop (then priority 2) and their INFO logging went through
  the blocking UART0 console at 115200 baud (lines 5 ms apart), so the DERP-map
  dump alone exceeded the 250 ms watchdog. Verified after the fix: 136 s uptime,
  `gap_ctrl_ms=40`, `gap_lowprio_ms=29480`, minimum free internal heap 98 KB.
- Fixed: MQTT never reached `mqtt_connected`. Cause: MicroLink's peer table had
  8 slots and the tailnet has 13 peers; the broker came 10th in the MapResponse,
  so the log showed `Peer table full (8 slots), cannot add <broker>` and no
  WireGuard tunnel to it ever existed. Both `config.max_peers` in
  `../esp32s3/ito_prototype_esp32s3/ito_tailnet_monitor.h` (changed on the
  owner's request) and `CONFIG_ML_MAX_PEERS` in `sdkconfig.defaults` are now 16;
  MicroLink clamps the former to the latter, and an existing `sdkconfig`
  overrides the defaults, so edit or regenerate it too. Verified on macOS: tunnel
  up over a direct path 18 s after boot, `tailnet=mqtt_connected` held,
  `gap_ctrl_ms=40`, minimum free internal heap 93 KB. If the tailnet grows past
  16 peers the broker can be dropped again; `CONFIG_ML_PRIORITY_PEER_IP` set to
  the broker's tailnet IP reserves it a slot.
- Fixed: after a reboot MQTT stayed in `mqtt_connecting` for minutes. Cause:
  MicroLink's `wireguard_tai64n_now()` stamps WireGuard handshakes with uptime, so
  a peer that saw the previous run drops every new handshake as a replay (DISCO
  pings still work; `tailscale ping` from the broker fails) until it forgets the
  idle peer. `main.cpp` now wraps it (`-Wl,--wrap=wireguard_tai64n_now` in
  `main/CMakeLists.txt`) to send NTP wall-clock TAI64N. Verified: reconnect about
  15 s after reboot, stable for 5 min.
- Open: on the PCB (powered, or unpowered but fed through the XIAO's 3V3 from
  USB) the I2C devices answer, and the control loop's bit-banged polls keep core 1
  busy; MicroLink's `ml_coord` and `ml_wg_mgr`, pinned to core 1, starve, the
  tunnel drops and `ito-diag-lo` goes silent. Safety monitoring is unaffected.
  Moving those two tasks to core 0 (wrapping `xTaskCreatePinnedToCore`) was tried
  and broke the board: it vanished from USB and the tailnet within about a minute
  of boot; not root-caused, as a flaky USB cable muddied the tests. Remaining
  options touch safety-reviewed code (faster GPIO in `bus.h`, or slower polling
  while monitor-only) and need the owner's go-ahead.
- `tailnet=starting` that never moves on means Wi-Fi has not associated (the
  task waits for `WL_CONNECTED` before time sync and MicroLink). If the log stops
  after `wifi:Set ps type` with no `state: init -> auth` lines, the AP was not
  found: check the antenna is attached, the SSID, and that it is on 2.4 GHz.
- Off the PCB the I2C bus floats, so the controller reports
  `mode=4 fault=STALE_SENSOR` and spends extra time in bounded bus waits. That is
  expected and not a defect.
