# Two remote SHT31 measurement points — revision L

J28 and J29 are two additional main-board ports for local differential I²C adapters. They are not direct long-cable connections. Each port requires one adapter beside the controller and another beside the remote sensor. PCA9615 adapters and SHT31 modules belong to the external BOM and are not fitted to the main PCB.

J26 is the existing short local I²C port. Leave it empty when using both remote SHT31s: all three ports share one bus and duplicate sensor addresses cannot coexist.

## Defined prototype arrangement

- Two independent links, each with two **SparkFun QwiicBus EndPoint COM-16988** modules: four modules total.
- Two **Adafruit SHT31-D breakout boards, product 2857**. These support 5 V power and I²C pull-ups. Use the Vin/GND/SDA/SCL solder connections; other module designs may need different supplies or level shifting.
- Two straight fully wired CAT5e/CAT6 RJ45 cables, initially 1 m each. Design target: up to 3 m per branch at ≤100 kHz, subject to qualification in the actual installation. Longer cables need new qualification; unlimited length is not guaranteed.
- Cables carry differential SCL/SDA and 5 V/GND. **Not Ethernet or PoE:** never connect these RJ45s to network equipment, switches or PoE injectors.
- Unbuffered controller-to-adapter wiring: approximately 10 cm maximum. Sensor-to-adapter: approximately 10–15 cm, with GND beside signals and verified edge rates. No long stubs or Y branches on the differential side.

## Main PCB: J28/J29

Both use five-pin JST XH, 2.50 mm, B5B-XH-A(LF)(SN). Use XHP-5 housings and suitable SXH-001T-P0.6 contacts. Numbering follows the PCB pin-1 mark. Verify cable continuity rather than trusting wire colors.

| Pin | Signal | Local EndPoint connection |
|---|---|---|
| 1 | +3V3, local logic/reference only | VCC/VDDA, local header marked 3.3V |
| 2 | GND | GND |
| 3 | SDA, 3.3 V | SDA |
| 4 | SCL, 3.3 V | SCL |
| 5 | Fused +5 V | VCC1/VDDB, separate power header |

F4 supplies J28:5 and F5 supplies J29:5; both are 250 mA fuses. Prototype consumption budget is ≤150 mA per complete branch; measure actual consumption. C51/C52 decouple the ports. The 5 V comes from M1, not the XIAO 3.3 V regulator. Reserve ≤300 mA on the 5 V rail for both branches plus the existing electronics; the overall 24 V input budget remains 4 A.

Pin 1 is not a 5 V input. Never short 3.3 V and 5 V together. With JP1 open, apply USB first and then 24 V; turn off in reverse order as described in ASSEMBLY.md.

## Jumpers on all four EndPoint modules

These settings were checked against SparkFun's official schematic. Check the revision actually received before soldering.

| Jumper | Two local modules | Two remote modules |
|---|---|---|
| `0-1` (VDDA–VDDB link) | **OPEN**, otherwise 5 V reaches the MCU rail | CLOSED: remote sensor bus operates at 5 V |
| `VCC1` | CLOSED, sends 5 V through cable | CLOSED, receives 5 V |
| `GND` | CLOSED | CLOSED |
| `I2C` pull-ups | Both traces OPEN; main-board R1/R2 provide pull-ups | Both traces CLOSED; local 4.7 kΩ pull-ups to 5 V |
| `PWR` LED | May be OPEN to reduce consumption/heat | OPEN to reduce heat near measurement point |

Retain each EndPoint's 390/100/390 Ω differential networks. Do not add separate 100 Ω termination resistors. Each branch is a point-to-point link with exactly two EndPoints.

Official RJ45 mapping: 1=DSCL−, 2=DSCL+, 3=GRN+, 4=VCC1, 5=GND, 6=GRN−, 7=DSDA−, 8=DSDA+. The green pair (3/6) is unused; do not apply external power there. Use a straight patch cable, not crossover. Prefer direct RJ45 cables between modules; this mapping is for continuity checks.

## Sensor end

Connect remote EndPoint VCC/VDDA (**5 V here**, even if PCB lettering says 3.3V) to SHT31 Vin, and connect GND/SDA/SCL. The chosen Adafruit board has 10 kΩ pull-ups to Vin; together with 4.7 kΩ adapter pull-ups this is approximately 3.2 kΩ. PCA9615 keeps the MCU side at 3.3 V.

- Sensor 1 on J28: ADR unconnected, using its onboard pulldown; address `0x44`.
- Sensor 2 on J29: ADR connected to its **own Vin**; address `0x45`.
- RST and ALR may remain unconnected for this prototype.
- Do not connect a 3.3 V-only Qwiic device to the remote 5 V sensor bus.

PCA9615 has no I²C address and does not change addresses. Ports are electrically parallel to the internal bus. A faulty external branch may disrupt internal measurements too; the bounded bus driver disables power on I²C faults. There is no galvanic isolation or per-port I²C multiplexer.

## Representative ambient measurements

Keep sensor heads outside hot heatsink exhaust, the cold boundary layer around the glass, direct sunlight and radiant heat. One meter separation is a starting point, not proof of representative ambient conditions. Compare both points with the installation off and during heating/cooling.

Use short thin wires to place each sensor approximately 10–15 cm away from its EndPoint board, inside a ventilated shield. Do not enclose sensor and warm adapter together in a sealed box. Avoid thermal contact with metal chassis, high-current cable bundles, adhesive or coating over the sensing aperture. Protect from droplets/condensation without obstructing air exchange. Keep the SHT31's internal heater off during ordinary measurements to avoid biasing temperature/RH.

Firmware reports the two points separately. Averaging two disturbed measurements does not make them correct. Choose the ambient reference for future automatic control only after physical comparison.

## Firmware L.1

`ambient` reports address, validity, age, temperature (°C), RH (%) and dew point (°C) for each sensor. Each sensor is sampled about once per second in single-shot high-repeatability mode without clock stretching. Both CRC bytes are checked. Firmware commands heater-off at initialization and after communication faults.

The Magnus approximation uses a=17.62 and b=243.12°C, over ambient −20…60°C and 0<RH≤100%. It estimates dew point over liquid water, not frost point over ice. Resolution is not absolute accuracy; displayed decimals support diagnostics.

Both sensors must have valid recent data before a power pulse (`REQUIRE_AMBIENT=true`). Starting requires age ≤1 s; an active test stops when data is invalid/stale, with maximum age 3.5 s. No additional SHT31 traffic occurs during a heater pulse of up to 2 s, to avoid disturbing software PWM. Automatic dew-point control is not implemented.

## Cable and measurement qualification

1. Unpowered: verify jumpers, cable continuity, 3.3/5 V separation and addresses.
2. Test one 1 m branch, then both. Measure remote supply voltage and check CRCs/fresh readings.
3. Verify all four PCA9615 VDDB pins remain within 3.0–5.5 V; target 4.5–5.25 V remotely. Local SDA/SCL high level must be about 3.3 V, never 5 V. Check low levels and rise times with a scope.
4. Repeat using the intended maximum cable length with TEC regulators/fan/heater active. Record faults, maximum data age and ADC/NTC continuity.
5. Disconnect one sensor/cable: missing data or faults must be visible and inhibit/stop power. Prefer unpowered reconnection; hotplug is not qualified.
6. Log at least 30 minutes with the installation off, then under thermal load. Reposition heads if installation heat shifts the ambient readings.

These tests have not been performed. Enter only actual measurements in `commissioning-results.csv`.

## Manufacturer references

- [NXP PCA9615](https://www.nxp.com/docs/en/data-sheet/PCA9615.pdf)
- [SparkFun EndPoint](https://www.sparkfun.com/sparkfun-qwiicbus-endpoint.html)
- [Official adapter schematic](https://github.com/sparkfun/QwiicBus_EndPoint/blob/main/Hardware/SparkFun_QwiicBus_Endpoint.sch)
- [Adafruit SHT31-D pinout](https://learn.adafruit.com/adafruit-sht31-d-temperature-and-humidity-sensor-breakout/pinouts)
- [Sensirion SHT3x-DIS](https://sensirion.com/media/documents/213E6A3B/63A5A569/Datasheet_SHT3x_DIS.pdf)
