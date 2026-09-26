# Prototype L firmware contract

The package contains PCB files, test instructions and compiled prototype firmware L.1 in `firmware/`. Host tests passed previously; hardware operation remains untested. Outputs must stay off with an unprogrammed XIAO or no MCU installed.

| XIAO label | Net / function |
|---|---|
| D0 | HOT_ADC, hot-side NTC |
| D1 | INPUT_I_ADC, total current from eFuse IMON; no longer a 3.3 V monitor |
| D2 | LOAD_PRESENT_N, low when voltage exists after relay |
| D3 | HEATER_PWM |
| D4 / D5 | SDA / SCL, 3.3 V |
| D6 | TEC_ARM; high inhibits heater driver |
| D7 | FAN_PWM transistor |
| D8 | FAN_TACH |
| D9 | POWER_ARM; AND with PGOOD requests relay |
| D10 | BUTTON_N |

I²C addresses: TCA9534=0x20; INA226=0x40; ADS1115=0x48; MCP4728=0x60 with the drawn address configuration. Verify actual devices before enabling power. TCA9534 P0–P3 are channel enables; P4 reads PROT_FAULT_N.

## Startup and faults

Immediately set POWER_ARM, HEATER_PWM and TEC_ARM low. Initialize I²C, write TCA output data zero first, then configure P0–P3 as outputs and P4 as input. Explicitly set every DAC channel to zero before EN or relay activation; do not trust stored DAC EEPROM values. Check all five temperature sensors, eFuse fault state and relay-off feedback. Use a watchdog and latched fault state: I²C timeout, invalid sensors and brownout must drop power enables.

R19 holds POWER_ARM low during reset, R33 holds HEATER_PWM low, R7 holds TEC_ARM low, and channel pulldowns keep LT8613s disabled. U15 qualifies relay requests with eFuse PGOOD. This is not a certified safety system.

Cooling sequence: heater PWM zero, confirm current decay, TEC_ARM high, then enable selected channels. Defogging sequence: DAC zero, channel EN low, confirm current decay with suitable external instrumentation during qualification, then TEC_ARM low and permit heater PWM. There is no separate digital current measurement for each TEC; do not assume unavailable feedback. Select a conservative switching delay only after scope verification on actual channels.

## Measurements and limits

IMON typically supplies 27.9 µA/A. R5=10 kΩ produces approximately 0.279 V/A; R4=0 Ω and R6/C3 provide connection and filtering. Use the correct nRF52840 ADC gain/reference and known resolution; do not blindly use `raw/4095*3.3`.

The used datasheet gain range over 2–6 A is 26.22–29.58 µA/A. Before current calibration, stop at no more than 1.00 V on INPUT_I_ADC, provided total ADC under-reading is demonstrably ≤3%; this assumption keeps actual current below 4 A. Then calibrate at two points and maintain a 4.0 A real-current ceiling with margin. Below 2 A this is not a precision ammeter. Average for regulation but retain fast fault detection; the eFuse supplies independent hardware limiting.

INA226 measures heater current only: 10 mΩ shunt, CAL=0x0800 for 0.25 mA/bit. Match averaging/conversion time to PWM. Do not hide peak current behind low average readings. Actual heater on-current must remain below 3.5 A. PWM reduces average current, not on-current.

TEC current: internal DAC reference 2.048 V, gain 1 and the 10 kΩ/10 kΩ divider yield up to approximately 1.024 V at ICTRL. Datasheet/shunt tolerances mean nominal 2 A is not a hard limit. Start at low codes; determine each channel's measured code limit at ≤2.0 A including ripple, overshoot and warm testing. Total input-current limits override cooling requests.

For initial glass tests use 10 W, provisional 40°C software stop and ≤2 s pulses with full temperature checks between pulses. Increase power/duration only after qualification. Start with 55°C hot-side software stop and an independent approximately 60°C NC thermostat, or lower limits when required by components. Invalid, open or shorted NTCs must be treated as faults, not very low temperatures.

Prototype firmware uses bounded software I²C at ≤100 kHz; R1/R2 are 2.2 kΩ. Use the selected XIAO board core's D-label constants instead of unchecked numeric Arduino pin numbers, which can differ between cores.

Ambient SHT31s use 0x44 and 0x45 without conflict with internal devices. Firmware checks CRCs, data age and validity; see REMOTE-SENSORS.md. Leave J26 empty with both remote sensors connected.
