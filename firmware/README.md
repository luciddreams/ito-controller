# Prototype firmware L.1

Supervised commissioning firmware for PCB L, XIAO BLE nRF52840 and the Mean Well 24 V supply. It provides short manual test pulses, not an automatic condensation cycle, BLE interface or unattended operation. No actual board has been tested with this firmware.

## Included files

- `ito_prototype/`: Arduino source and `config.h`.
- `build/`: compiled diagnostic version with power tests blocked.
- `calibrate.py`: two-point calculations using actual measured data.
- `tests/`: host tests for safety limits and real firmware states using simulated hardware. The test configuration is for tests only.
- `verification.txt`: previous actual build/test results.

USB command `status` reports raw ADC counts, input current, heater current, bus voltage, five temperatures, fan speed and fault state. Temperatures and input current are indicative until calibrated. States: 0=idle, 1=relay check, 2=heater, 3=TEC, 4=fault.

## Build and flash

Use Arduino IDE with **Seeed nRF52 Boards 1.1.13**, board **Seeed XIAO nRF52840**. Do not use the mbed, Sense or Plus variants for this package. TinyUSB 1.7.0 comes with this core; no additional sensor libraries are needed.

Board index: https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json

Open `ito_prototype/ito_prototype.ino`, or use Arduino CLI:

```sh
arduino-cli core update-index --additional-urls https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
arduino-cli core install Seeeduino:nrf52@1.1.13 --additional-urls https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
arduino-cli compile --fqbn Seeeduino:nrf52:xiaonRF52840 --output-dir build ito_prototype
arduino-cli upload --fqbn Seeeduino:nrf52:xiaonRF52840 --port YOUR_PORT --input-dir build
```

On Linux this core may require `adafruit-nrfutil` on PATH: `python -m pip install adafruit-nrfutil`. Alternatively, double-press reset and copy the UF2 to the bootloader drive. Flash only with 24 V off and JP1 OPEN. Keep USB connected while JP1 is open and 24 V is on; switch off 24 V and discharge the load bus before removing USB. See ASSEMBLY.md.

## Initial diagnostics

Do not connect glass or TECs yet. Follow PROTOTYPE-START.md, connect the five correctly specified NTCs and verify the NC safety chain. Begin with power resistors, a current-limited lab supply, external meters and an oscilloscope.

Firmware drives heater PWM, TEC_ARM and POWER_ARM low before initialization. TCA output latches are zeroed before changing direction; DAC channels are explicitly set to zero with the internal 2.048 V reference. The fan receives a continuous 100% request. Sensor failures, excessive temperature/current, I²C faults, altered configuration registers, releasing the hold button or USB loss during a test disable direct power enables and latch a fault. Tests never restart automatically.

I²C uses a bounded open-drain software driver on D4/D5 at ≤100 kHz. Clock-stretch timeout is 1 ms; transaction timeout is 20 ms. This avoids unbounded Wire waits in this core. The nRF watchdog has a nominal 250 ms timeout and is fed only after a complete loop. Verify software timing and fault response with a scope; this is not a certified safety controller.

## Calibration before enabling power tests

`MEASUREMENTS_VERIFIED=false`, zero TEC code limits and an unspecified heater resistance block tests by default. Set the flag true only after entering relevant measurements and completing the checks below. For the first resistor test, unused TEC channels may retain zero limits. Limit the lab supply and begin with a very small DAC code; increase code limits only from external measurements. A code limit is not a predicted TEC current.

1. Measure 3.3 V and enter `VDD_V`. Verify the exact R25 and beta of all five NTCs; 3950 K in config.h is a placeholder. Verify readings at ambient and a second relevant temperature. Open/shorted probes must cause faults. Use the proper transfer function if a probe does not fit the beta approximation.
2. Calibrate the ADC with two known voltages on the isolated measurement input, controller dry and power stages off. Disconnect the original signal source first; do not inject voltage against IMON or an attached probe. Nominal ADC scaling is 3.6 V/4096, not 3.3 V/4095. Check both ADC channels; `status` reports `raw=INPUT,HOT`.
3. Run `python calibrate.py adc RAW1 VOLT1 RAW2 VOLT2`. Perform an independent third check. Enter slope/offset in config.h. Under-reading must remain demonstrably below 3% over the relevant range, including temperature and margin.
4. Calibrate IMON using a supervised external electronic load on +24_PROT without heater/TECs. Measure two points in the 2–4 A range. Run `python calibrate.py current IMONV1 AMP1 IMONV2 AMP2` and check a third point. Firmware stops at 1.00 V or 3.8 A calibrated current; actual current must remain below 4 A.
5. Verify INA226 against an external meter and scope. Firmware reads shunt voltage directly: 2.5 µV/bit across 10 mΩ. Software limit is 3.4 A for margin below 3.5 A. Examine pulse peaks; an averaging meter alone is insufficient.
6. Measure full-busbar heater resistance, or the initial dummy resistor. Set `HEATER_RESISTANCE_OHM`. Firmware uses measured voltage to calculate a 10 W duty ceiling and check expected on-current. A dummy load must use its own resistance value.
7. Calibrate one TEC channel at a time using a resistor, scope and current meter. Set conservative per-channel `TEC_MAX_CODE` values that keep current including ripple, warm operation and overshoot below 2 A. Keep unqualified channels at zero. Firmware L.1 activates at most one TEC at a time.
8. Check fan pulses/revolution and minimum speed. The fan must already be running before a test. Test a disconnected tach wire. Verify the 2 s off interval with a scope: current must have decayed before a new mode starts.

## Running a test

USB terminal: 115200 baud, newline. J5 is the hold-to-run button; keep it pressed through relay startup and the entire pulse. Releasing it stops an active test and latches a fault. All NC safety contacts must be operational.

| Command | Action |
|---|---|
| `status` | Diagnostics only; no power action |
| `heat 20 500` | Heater duty 20/255 for 500 ms, only within calculated 10 W ceiling |
| `tec 1 100 500` | Channel 1, DAC code 100, 500 ms, only within the calibrated channel code limit |
| `stop` | Disable direct GPIO enables, then zero TCA and DAC |
| `reset` | Requires button released; reinitializes devices and clears faults only with healthy measurements and relay off |
| `ambient` | Remote temperature, RH, dew point, validity and age |

These examples are not calibrated current settings. Each test waits 200 ms for the relay/bus before enabling the output. Pulses are 100–2000 ms with at least 2 s off between tests. Initial heater PWM is 10 Hz. Never respond to a fault merely by raising a limit. If a previous-cycle fault remains, inspect `status` before `reset`.

Firmware monitors five probes but cannot reliably detect a mechanically detached yet electrically valid NTC. Physically verify bonding and temperature uniformity. Independent thermostats remain necessary.

## Outstanding physical tests

Record actual measurements in `../commissioning-results.csv`: open/shorted I²C, watchdog, reset in each mode, sensor faults, button/USB loss, relay failure, fan stall, current limits and thermal tests. Host tests and a successful build replace none of these. Four simultaneous TECs and automatic defogging are later firmware work after single-channel qualification.

## Two ambient measurements

Connect remote SHT31s according to ../REMOTE-SENSORS.md. Both are required by default before pulses; missing/stale values block startup. They are supplied by the external 5 V branch and will be absent with USB only and 24 V off. This is expected and leaves power disabled. REMOTE-SENSORS.md covers addresses, CRC, timing and the dew-point approximation.
