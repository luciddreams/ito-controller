# ITO controller

KiCad hardware and firmware for the ITO glass heater/TEC controller.

- `firmware/esp32s3/ito_prototype_esp32s3`: the Arduino IDE build and the shared,
  safety-reviewed controller sources. Do not change them without being asked.
- `firmware/tailscale`: the ESP-IDF 5.5.1 + MicroLink telemetry monitor. Read
  `firmware/tailscale/CLAUDE.md` before building, flashing or editing it.
- Credential headers (`firmware/tailscale/main/tailnet_local*.h`,
  `firmware/esp32s3/ito_prototype_esp32s3/wifi_local.h`) and all firmware build
  outputs embed secrets: never print, diff, commit or upload them.
