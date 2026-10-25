# Revision L: XIAO ESP32S3 Arduino port

Diagnostic-first port of the original revision-L firmware at commit
baf16fa3a47d1587ad8220757d5e765ad7660932. This is not revision M.
Hardware operation has not been verified. Delivered configuration blocks all
power pulses until measurements and calibration are completed. Only U11 is
marked installed; U12–U14 are excluded. Heater tests are disabled.

## Arduino IDE

Install **esp32 by Espressif Systems 3.3.2** through Boards Manager. Package URL:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

Extract the entire directory and open
`ito_prototype_esp32s3/ito_prototype_esp32s3.ino`; keep its header files alongside it.
Select **XIAO_ESP32S3**, **USB Mode: USB-OTG (TinyUSB)**,
**USB CDC On Boot: Enabled**, 8 MB flash, PSRAM disabled. No additional libraries
are required. Other USB settings deliberately fail compilation.

First upload with the module removed from the controller PCB, powered only by
USB. Open Serial Monitor at 115200 baud, newline ending. Send `status`, `pins`,
and `ambient`. Missing-device faults are expected off-board. If upload fails,
hold BOOT while connecting USB, release BOOT, select the new port and upload.
Do not run generic Blink or factory examples while mounted on a powered PCB.

## Connection and startup cautions

- For USB-powered commissioning, remove **JP1**: it connects PCB 5V_AUX to the
  XIAO 5V/VBUS pin. Seeed requires a series diode for external 5V input. Do not
  assume JP1 alone provides power-source isolation. Standalone PCB powering
  needs its power path checked and suitable isolation implemented first.
- Mount/unmount only with USB and PCB power disconnected. Check orientation
  against the PCB schematic. Supply XIAO by USB before applying controller
  power; remove controller power and let it discharge before disconnecting USB.
- **D6 = GPIO43 = UART0 TX = TEC_ARM.** ROM boot messages can drive this pin
  before setup, including after watchdog resets. USB serial selection does not
  suppress the ROM. Firmware preloads low output latches at setup but cannot
  guarantee an inactive pin before that. Verify K1, its hardware interlocks,
  channel enables and pulldowns. Never bypass the relay with a TP1 bridge.
  Do not connect TEC/heater loads until startup/reset behaviour is measured.
  No eFuse changes are part of this port.
- **D2 = GPIO3 = LOAD_PRESENT_N** also selects the JTAG source at reset. It is
  used as an input; firmware uses USB-OTG rather than USB Serial/JTAG. Check
  both relay states on hardware; software testing cannot establish boot safety.
- GPIOs are 3.3V signals. The 3V3 pin is the module regulator output. Check
  attached signal levels and regulator load; do not apply external 5V to GPIOs.
- A11/A12 are not ADC-capable on this chip. This port uses only A0/A1.
- Use the standard XIAO ESP32S3 without a Sense expansion board: SD connections
  on D8/D9/D10 would conflict with fan tach, power arm and the dead-man button.
- Wi-Fi, Bluetooth, camera, SD and battery operation are not used or tested.

Previous F11 readings around 20.9V with no MCU require checking the relay path.
An energized load rail while the relay is commanded off is not a successful
commissioning result. Investigate a bypass, relay state or wiring issue; do not
relax the firmware's relay-stuck check.

## Pin mapping

| XIAO | GPIO | Revision-L signal |
|---|---:|---|
| D0/A0 | 1 | HOT_ADC |
| D1/A1 | 2 | INPUT_I_ADC |
| D2 | 3 | LOAD_PRESENT_N |
| D3 | 4 | HEATER_PWM |
| D4 | 5 | SDA |
| D5 | 6 | SCL |
| D6 | 43 | TEC_ARM |
| D7 | 44 | FAN_PWM (low transistor drive requests full fan speed) |
| D8 | 7 | FAN_TACH |
| D9 | 8 | POWER_ARM |
| D10 | 9 | BUTTON_N |

## Port and commissioning

The port retains the original bounded software I2C transactions, safety checks,
DAC/expander readback and timed pulse state machine. ADC input voltage uses
`analogReadMilliVolts`, with independent correction gains and offsets, rather
than the nRF52840 raw-count scaling. Upper-range ADC clipping rejects the
measurement. The task watchdog has a 250 ms timeout and is fed only at the end
of the control loop. Tach counting uses ESP32 critical sections. Serial output
is bounded and nonblocking; diagnostics may be dropped when USB cannot keep up.
Heater PWM remains the original 10 Hz software PWM; no LEDC dependency exists.

`status` reports raw counts, millivolts, installed mask and verification flags.
`pins` reports interlock/output/I2C levels. `ambient` reports SHT31 readings.
`stop` cancels a pulse; `reset` attempts fault recovery and never restarts one.

Before permitting pulses, verify the actual sensors, five thermistors, fan,
dead-man button, efuse fault, relay feedback and current measurement against the
revision-L commissioning procedure. INA226 measures heater current, not U11
current. Do not substitute guessed calibration constants.

In `config.h`, `MEASUREMENTS_VERIFIED` and `ADC_VERIFIED` default false;
`TEC_MAX_CODE` entries default zero. Temperature and current conversion values
are placeholders pending measurement. The inherited configuration gate also
requires a measured valid heater resistance. Keep `ALLOW_HEATER=false` and
`INSTALLED_TEC_MASK=0x01` for the present partially assembled board.

For each ADC channel separately, measure two known voltages at the ADC input
with a meter and compare the uncorrected millivolts in status. With voltages in
volts, gain = (meter2-meter1)/(adc2-adc1), offset = meter1-gain*adc1.
Validate an independent third point and the operating range before setting the
verification flag. Validate the current conversion and NTC values separately.

Only after hardware validation and calibration can `tec 1 CODE MS` request a
bounded pulse. CODE must be an experimentally established limit, not a guessed
current. Pulses require the dead-man button, connected USB terminal and all
interlocks. U12–U14 requests are rejected with the delivered installed mask.
This is commissioning firmware, not autonomous temperature regulation.

## Reproduce verification

From this directory: `sh tests/run.sh` (requires g++).

With Arduino CLI and core 3.3.2 installed:

```sh
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32S3:USBMode=default,CDCOnBoot=default --warnings all ito_prototype_esp32s3
```

Note: core 3.3.2 uses menu value `CDCOnBoot=default` for Enabled, and `cdc` for
Disabled. `USBMode=default` means USB-OTG (TinyUSB).

Sources checked 2026-10-25:
- https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/
- https://docs.espressif.com/projects/arduino-esp32/en/latest/api/adc.html
- https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/system/wdts.html

## L.2-S3 Wi-Fi telemetry and Next.js dashboard

See [dashboard/README.md](../../dashboard/README.md) for setup.
Copy the sketch's `wifi_local.example.h` to `wifi_local.h` and enter credentials
locally to enable Wi-Fi. Send `wifi` over USB to read the assigned IP. A separate
core-0 task serves authenticated, read-only `GET /api/status` snapshots. ADC/I2C
and GPIO access stay in the main loop. The snapshot has sequence, sample ages,
validity flags and null values for nonfinite measurements.

When Wi-Fi is compiled in, **all power pulses are blocked**, including over USB,
regardless of calibration settings. This is a commissioning monitor until Wi-Fi
scheduling and watchdog behaviour are verified on physical hardware. Without the
local header, the existing USB-only behaviour remains. Network disconnects never
issue control commands. The dashboard runs locally on your PC, not on the XIAO.
