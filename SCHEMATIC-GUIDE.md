# Reading the schematic — revision L

Open `ito-controller.kicad_pro` in KiCad 9, then the schematic editor. The root sheet opens the 13 functional sheets. The PDF has the same circuit on 14 landscape A3 pages and remains sharp when zoomed.

## Page map

| PDF page | Circuit |
|---|---|
| 1 | Functional overview |
| 2 | MCU, 5 V and USB |
| 3 | R7B input and protection |
| 4 | Relay and hardware shutdown |
| 5 | ITO heater |
| 6 | DAC and current setpoints |
| 7 | TEC channel enables |
| 8–11 | Four matching TEC current regulators |
| 12 | Glass temperature measurement |
| 13 | Heatsink, humidity and fan |
| 14 | Two remote ambient ports |

## Conventions

Signals and power generally flow left to right. Components are connected by actual KiCad wires. A dot marks a junction; a crossing without a dot is not connected. Ground uses conventional GND symbols. Supply rails are wired within each page; matching power symbols connect pages.

Global labels retain original net names and join matching nets across sheets. Internal nets also retain their names for comparison with the PCB, so some labels appear twice on one wired connection. Blue crosses mark intentionally unconnected pins.

Physically parallel IC pins share stacked drawing positions, including LT8613 VIN/SW/GND and TPS26630 IN/OUT/GND. Notes on the relevant page list all numbers. Parallel relay contacts are also identified. Every physical pin remains in the netlist.

References are native editable KiCad reference fields. Concise displayed values use the `Schematic value` custom field. The original full `Value` is hidden but retained with footprint, manufacturer and part number. If changing a component value later, update both fields. Legacy Value strings and net identifiers are retained for PCB/BOM compatibility; display text is English.

## Verification

The original redraw compared all 202 physical components and 638 electrical pins against the original netlist: connected/unconnected status, exact net names, values, footprints and component identity matched. See `schematic-redraw-validation.json` for that historical check and `english-localization-validation.json` for the English update. ERC results are in `schematic-erc.rpt`.

English localization changes text only. The prototype's operating limits and commissioning requirements still apply.
