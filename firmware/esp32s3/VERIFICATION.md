# Verification — 2026-10-25

- Arduino CLI 1.3.1, esp32:esp32 core 3.3.2.
- Real target: esp32:esp32:XIAO_ESP32S3:USBMode=default,CDCOnBoot=default.
- Final compile with --warnings all: passed, no warnings in final incremental build.
  Initial full core compilation produced upstream ESP-IDF initializer warnings.
- Flash: 340411 bytes; static RAM: 41752 bytes.
- Negative configuration check: CDC disabled is rejected at compile time.
- Four host suites compiled with g++ -Wall -Wextra -Werror and passed:
  safety, controller state transitions, ambient calculations/CRC, delivered defaults.
- Coverage includes pulse expiry, button and USB loss, I2C failure, sensor faults,
  stale measurements, omitted TEC channels, watchdog failure, ADC clipping,
  malformed commands, DAC/expander readback and blocked USB output.

No physical XIAO or controller was connected to the build environment. ADC
accuracy, electrical timing, startup/reset pulses, hardware watchdog recovery,
relay operation, current calibration and U11 operation remain unverified.
Defaults therefore block all power pulses. In particular GPIO43 boot-ROM UART
activity requires physical interlock/startup verification, not just unit tests.
