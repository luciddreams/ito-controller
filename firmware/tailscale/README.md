# Experimental ITO Tailscale + MQTT monitor

This is a separate ESP-IDF 5.5.x build with Arduino-ESP32 3.3.2 as a component.
The existing Arduino/HTTP build remains available. This integration has not yet
been compiled for the target or tested on a physical XIAO. Do not flash until
the target build passes. Do initial testing off-board, USB only, with the antenna.

The controller runs on core 1. A separate task on core 0 joins Wi-Fi, synchronizes
time, joins Tailscale using MicroLink, resolves the broker as a known tailnet peer,
and publishes the existing schema once per second using MQTT 3.1.1 QoS 0.
No MQTT subscriptions, power commands, or HTTP control endpoints are installed.
`monitor::enabled` is always true, including when disconnected, so the existing
configuration gate rejects heater and TEC pulses. The 250 ms watchdog is unchanged.
Flash/crypto scheduling and network-loss recovery still require bench validation.

## Build

Install ESP-IDF 5.5.1 and use its activated terminal. From this directory:

```text
python bootstrap.py
```

Copy `main/tailnet_local.example.h` to `main/tailnet_local.h`, then configure Wi-Fi,
Tailscale enrollment, unique device/client names, and the broker account locally.
The MQTT host defaults to `luciddreams.tail8decad.ts.net` and port 1883, but confirm
the actual listener before deploying. Plain MQTT here is inside WireGuard; no
ordinary-DNS or public/LAN fallback is implemented. MQTT-over-TLS (8883) is not
implemented by this transport. Do not point this configuration at a TLS listener.

```text
idf.py set-target esp32s3
idf.py build
idf.py -p YOUR_SERIAL_PORT flash monitor
```

Send `wifi` to the serial console for connection state (e.g. `mqtt_connected`).
Broker connection success is not proof that topic ACLs allow publishing: verify
receipt on the server. Give the device publish-only access to
`ito/controller-01/telemetry`; give the dashboard subscribe-only access.

## Enrollment and compatibility

MicroLink is an independent Tailscale-compatible implementation, not an official
Tailscale ESP32 client. It is pinned by bootstrap.py rather than tracking main.
Use a narrowly scoped device tag, allow only the MQTT destination/port in your
tailnet policy, and keep all secrets out of Git/chat. The configured header and
its secrets are embedded in the firmware binary: keep that binary private too.
Store any enrollment key locally. Node identity is persisted by MicroLink in NVS.
Tailnet Lock is not supported by the community integration; verify compatibility
with your tailnet before enrollment. Do not change existing tailnet policy blindly.

The XIAO ESP32S3's octal PSRAM must be enabled. NTP, outbound HTTPS, and either
direct UDP or DERP must be available. If waiting at `time_sync`, check NTP access.
Do not erase NVS as a routine recovery: it removes device identity as well as
other saved data. An expired or revoked enrollment key may require local renewal.

## Remaining verification

- ESP-IDF target compile (the PR workflow builds placeholder credentials).
- Physical startup, PSRAM availability, enrollment and broker authorization.
- Direct and DERP MQTT delivery; Wi-Fi/broker outages and device/server reboot.
- Watchdog stability with the unchanged sensor loop and simultaneous VPN crypto.
- Tailnet access restricted to the intended broker; no public MQTT exposure.

See `dashboard/DEPLOY-TAILNET.md` for the receiving dashboard container.
