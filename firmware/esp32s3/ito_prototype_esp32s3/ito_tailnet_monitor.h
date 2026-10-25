#pragma once
#include "telemetry.h"
#include "tailnet_local.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <microlink.h>
#include <time.h>

namespace monitor {
// Never condition this on connectivity: offline builds must also reject pulses.
constexpr bool enabled = true;
static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
static telemetry::Snapshot latest;
static uint32_t published = 0;
static bool hasSnapshot = false;
static char connection[120] = "tailnet=starting monitor_only=1\n";

inline void publish(const telemetry::Snapshot& snapshot) {
  portENTER_CRITICAL(&mux);
  latest = snapshot; published = millis(); hasSnapshot = true;
  portEXIT_CRITICAL(&mux);
}
inline void status(char* out, size_t n) {
  portENTER_CRITICAL(&mux); snprintf(out, n, "%s", connection);
  portEXIT_CRITICAL(&mux);
}
inline void state(const char* value) {
  portENTER_CRITICAL(&mux);
  snprintf(connection, sizeof(connection), "tailnet=%s monitor_only=1\n", value);
  portEXIT_CRITICAL(&mux);
}

// PubSubClient transport: every byte goes through MicroLink, never LAN fallback.
class TailnetClient : public Client {
  microlink_t* link;
  microlink_tcp_socket_t* socket = nullptr;
  int buffered = -1;
public:
  explicit TailnetClient(microlink_t* value) : link(value) {}
  int connect(const char* host, uint16_t port) override {
    stop();
    uint32_t ip = microlink_resolve(link, host);
    // MQTT is restricted to a resolved tailnet peer, not ordinary DNS.
    if (!ip || !microlink_is_connected(link)) return 0;
    socket = microlink_tcp_connect(link, ip, port, 15000);
    return socket != nullptr;
  }
  int connect(IPAddress, uint16_t) override { return 0; }
  size_t write(uint8_t value) override { return write(&value, 1); }
  size_t write(const uint8_t* bytes, size_t n) override {
    if (!socket || microlink_tcp_send(socket, bytes, n) != ESP_OK) {
      stop(); return 0;
    }
    return n;
  }
  int available() override {
    if (buffered >= 0) return 1;
    if (!socket) return 0;
    uint8_t byte;
    int n = microlink_tcp_recv(socket, &byte, 1, 1);
    if (n < 0) stop();
    else if (n == 1) buffered = byte;
    return buffered >= 0 ? 1 : 0;
  }
  int read() override {
    if (!available()) return -1;
    int value = buffered; buffered = -1; return value;
  }
  int read(uint8_t* bytes, size_t n) override {
    size_t count = 0;
    while (count < n && available()) bytes[count++] = uint8_t(read());
    return int(count);
  }
  int peek() override { return available() ? buffered : -1; }
  void flush() override {}
  void stop() override {
    if (socket) microlink_tcp_close(socket);
    socket = nullptr; buffered = -1;
  }
  uint8_t connected() override {
    return socket && microlink_is_connected(link) && microlink_tcp_is_connected(socket);
  }
  operator bool() override { return connected(); }
};

inline void task(void*) {
  WiFi.persistent(false); WiFi.mode(WIFI_STA);
  WiFi.setHostname(ITO_TAILSCALE_NAME);
  WiFi.setAutoReconnect(true); WiFi.begin(ITO_WIFI_SSID, ITO_WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) vTaskDelay(pdMS_TO_TICKS(250));
  state("time_sync");
  configTime(0, 0, "time.cloudflare.com", "pool.ntp.org");
  while (time(nullptr) < 1704067200) vTaskDelay(pdMS_TO_TICKS(500));
  microlink_config_t config = {};
  config.auth_key = ITO_TAILSCALE_AUTH_KEY;
  config.device_name = ITO_TAILSCALE_NAME;
  config.enable_derp = config.enable_stun = config.enable_disco = true;
  config.max_peers = 16;
  microlink_t* link = microlink_init(&config);
  if (!link || microlink_start(link) != ESP_OK) {
    state("init_failed"); vTaskDelete(nullptr); return;
  }
  TailnetClient transport(link);
  PubSubClient mqtt(transport);
  mqtt.setServer(ITO_MQTT_HOST, ITO_MQTT_PORT);
  mqtt.setKeepAlive(15); mqtt.setSocketTimeout(2);
  if (!mqtt.setBufferSize(2560)) {
    state("buffer_failed"); vTaskDelete(nullptr); return;
  }
  uint32_t lastSend = 0, lastSequence = 0, retryAt = 0, retryDelay = 1000;
  bool wifiWasUp = true;
  for (;;) {
    bool wifiUp = WiFi.status() == WL_CONNECTED;
    if (!wifiUp) {
      transport.stop(); wifiWasUp = false; state("wifi_disconnected");
    } else {
      if (!wifiWasUp) {
        microlink_rebind(link); wifiWasUp = true; retryAt = millis();
      }
      if (!microlink_is_connected(link)) {
        transport.stop(); state("connecting");
      } else if (!mqtt.connected()) {
        state("mqtt_connecting");
        if (int32_t(millis() - retryAt) >= 0) {
          // No subscriptions or command callbacks: telemetry only.
          if (mqtt.connect(ITO_MQTT_CLIENT_ID, ITO_MQTT_USER, ITO_MQTT_PASSWORD)) {
            retryDelay = 1000; state("mqtt_connected");
          } else {
            transport.stop(); retryAt = millis() + retryDelay;
            retryDelay = retryDelay < 30000 ? retryDelay * 2 : 60000;
          }
        }
      } else if (mqtt.loop() && uint32_t(millis() - lastSend) >= 1000) {
        lastSend = millis();
        telemetry::Snapshot copy; uint32_t stamp; bool valid;
        portENTER_CRITICAL(&mux);
        copy = latest; stamp = published; valid = hasSnapshot;
        portEXIT_CRITICAL(&mux);
        if (valid && uint32_t(millis() - stamp) < 2000 && copy.sequence != lastSequence) {
          char json[2048];
          if (telemetry::encode(copy, json, sizeof(json)) &&
              mqtt.publish(ITO_MQTT_TOPIC, json, false)) {
            lastSequence = copy.sequence; state("mqtt_connected");
          } else { transport.stop(); state("publish_failed"); }
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}
inline void begin() {
  if (xTaskCreatePinnedToCore(task, "ito-tailnet", 12288, nullptr, 1, nullptr, 0) != pdPASS)
    state("task_failed");
}
}
