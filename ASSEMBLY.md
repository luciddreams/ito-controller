# Assembly and external wiring

## Components and mechanics

`BOM-prototype.csv` contains one row per component position. MPNs specify the electrical implementation; stock and minimum order quantities have not been checked. Generic passives may be substituted only with the same value, footprint and at least the specified voltage, power and tolerance. Do not replace U1, Q2/Q3, shunts, inductors, KPS output capacitors or precision dividers without review.

- XIAO BLE nRF52840: two Samtec SSW-107-01-G-S sockets, 15.24 mm row spacing and 2.54 mm pitch. USB faces the lower PCB edge. Align sockets with the unpowered module; remove the module before reflow or power testing.
- Populate JP1's header but leave its shunt **OPEN** by default. Do not parallel USB power and +5_AUX without control. During USB-powered tests, switch off 24 V and discharge the load bus before disconnecting USB, preventing IMON from powering an unpowered MCU input. For standalone operation: disconnect USB, close JP1 while unpowered, then apply 24 V.
- Reserve at least 22 mm height plus clearance for C5 and 5.5 mm plus clearance for KPS capacitors. Do not press enclosure parts against them.
- Q1/Q2 are upright TO-220 devices, G-D-S according to the footprint. Their metal tabs are electrically connected to drain. Keep them away from conductive chassis parts. Q2 should not nominally need a separate heatsink at 4 A; measure its temperature. Q1 cooling depends on the measured PWM/heater load.
- Use insulating spacers at the four M3 mounting holes. Keep the enclosure/heatsink clear of PCB copper. Do not direct hot TEC exhaust over the PCB.
- Electrically isolate the reflective metal coating from the ITO heater. Provide strain relief for wires attached to the 0.5 mm copper busbars; do not transfer cable loads to the glass.

## Mean Well GST160A24-R7B directly into J30

Fit **J30 = Kycon KPJX-4S**, the unshielded four-pin through-hole socket. The supply's R7B plug connects directly at the lower board edge. No adapter or external panel socket is required. Order exactly KPJX-4S; other suffixes, including -S/-PM variants, have different mechanics.

| Manufacturer pin | Function |
|---|---|
| 1 and 4 | +24_IN; both connected |
| 2 and 3 | GND / return; both connected |

Numbers follow the manufacturer markings. The footprint mirrors the bottom view in Kycon drawing A13. Verify the actual component and polarity before applying power. The connector front extends 4 mm beyond the board; allow plug and latch clearance. Its specified rating is 7.5 A/contact at 48 V DC. The **PCB remains a ≤4 A prototype**; connector capacity does not qualify higher system power.

J3 is an alternative two-pin lab-supply input, directly parallel to J30. Both are live when either is powered. **Never connect two supplies simultaneously.** J3 pin 1 = +24_IN, pin 2 = GND. Its cable entrance faces the free space above the terminal.

The previously discussed DC PLUG-P1J-R7B adapter has the wrong direction for this supply. The reverse R7BF-P1J adapter is also unnecessary in this design. Mean Well specifies -V connected to AC FG; account for this when connecting USB and grounded instruments.

## Other connections

`connector-pinout.csv` is authoritative for physical pin numbers. Phoenix 1715721 terminals use 5.08 mm pitch, JST-XH signal connectors 2.50 mm and XIAO headers 2.54 mm; they are not interchangeable.

- J4: heater busbars, preferably short twisted flexible 1.0 mm² wires.
- J11–J14: separate TEC channels; at least 0.5 mm² per conductor for these 2 A tests. Do not parallel TECs on one output. Identify hot/cold polarity at low current.
- J5: normally-open hold-to-run button to GND. J9: separate normally-open eFuse reset button, SHDN to GND. J9 is not an emergency stop.
- J6: NC emergency stop. J7: NC glass thermostat. J8: NC heatsink thermostat. Contacts must suit 24 V DC and relay-coil current. Manual-reset thermostats are preferable. Automatically resetting contacts require a latched firmware fault and do not replace supervised testing.
- J21–J24: four 10 kΩ glass NTCs; J25: one 10 kΩ hot-side NTC. Select a specified beta curve and configure firmware accordingly; 10 kΩ alone does not define the curve. Measure every probe before bonding it.
- J26: short local 3.3 V I²C humidity-sensor connection. Use a 3.3 V module without 5 V pull-ups and verify cable pinout. Leave J26 empty when both remote SHT31s are fitted.
- J27: 24 V fan, maximum 0.5 A for the prototype. Tach must be open-collector/open-drain. PWM is a transistor sinking the control line to GND, not a 24 V logic output. Verify the fan's actual pin sequence and wire an adapter; a PC-style plug does not prove 24 V compatibility.

Known 10 kΩ resistors may replace NTCs during initial electronic tests only. Actual correctly mounted temperature sensors are required for glass/TEC thermal testing.

XIAO top view: J1 is right, x=144.29 mm; J2 is left, x=129.05 mm; both pin 1 positions are y=104 mm. Row spacing is 15.24 mm. Module components face upwards, USB towards the lower edge.

## Additional ambient ports in L

Populate J28/J29, F4/F5 (250 mA) and C51/C52 (1 µF). Follow [REMOTE-SENSORS.md](REMOTE-SENSORS.md) for four external EndPoint modules and two SHT31s. These are local 3.3 V I²C ports with separate 5 V power, not direct long-cable ports.
