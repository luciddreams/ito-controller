# Revision M design review

## Sources and pinout checks

Manufacturer documentation checked on 2026-10-11:

1. [TI TPS2663x Rev G](https://www.ti.com/lit/ds/symlink/tps2663.pdf): PWP20 pinout, package drawing PWP0020T, 2× overload pulse, reverse-blocking driver, ILIM, IMON and PGOOD behavior.
2. [ADI LT3741/LT3741-1 Rev G](https://www.analog.com/media/en/technical-documentation/data-sheets/37411fg.pdf): **FE20** pinout (different from UF/QFN), non-sinking -1 variant, CTRL range, 51 mV current reference, 1.21 V FB, RT table, compensation, current-sense filtering, gate-driver bypass and MOSFET guidance.
3. [Infineon IRLR024N](https://www.infineon.com/assets/row/public/documents/24/49/infineon-irlr024n-datasheet-en.pdf): 55 V VDS, 65 mΩ maximum at 5 V, 16 nC gate charge, DPAK pin1 G / pin2 D / pin3 S. Custom land uses extended gate/source solder pads; confirm the actual ordered package against the fabrication drawing.
4. [TI INA180 Rev H](https://www.ti.com/lit/ds/symlink/ina180.pdf): use **A1**, not B1, because A/B pinouts differ. A1 is gain20 with pin1 OUT,2 GND,3 IN+,4 IN−,5 VS.
5. [Bourns SRP1265A](https://www.bourns.com/docs/product-datasheets/srp1265a.pdf): 330M is 33 µH ±20%, DCR max58 mΩ, Irms8 A, Isat11 A; 14.2 mm pad envelope,8 mm central gap,5 mm pad height.

Ordering numbers and stock are separate checks. Search results showed a Mouser listing for LT3741EFE-1#PBF, but stock of the entire BOM has not been reserved or exhaustively verified. Do not order a UF/QFN variant or substitute TPS26633 for TPS26631 based on family name alone.

## TEC power-stage review

The power path is VIN → upper MOSFET → SW → 33 µH → 25 mΩ → output capacitors → connector. The lower MOSFET connects SW to ground. The controller has separate Kelvin taps from the shunt pads, followed by matched10 Ω resistors and33 nF differential filtering. Four INA180s preserve the nominal0.5 V/A testpoint scale. They do not add automatic per-channel firmware protection.

At24 V and approximately11.94 V, calculated inductor ripple is0.606 A peak-to-peak at33 µH/300 kHz. At28 V withL−20% and an assumed frequency−10%, it is0.961 A peak-to-peak. Frequency−10% is an analysis assumption, not a guaranteed oscillator specification at300 kHz. The current-loop peak limiter is above the average setpoint. Inductor ripple must not be mistaken for the much smaller TEC-lead ripple after the output capacitors.

Selected RT is143 kΩ. The nominal compensation formula gives roughly33.2 kΩ and6.67 nF. The installed22.1 kΩ/6.8 nF pair is a conservative starting point, not a measured stability result. Validate CC operation, CV entry, open load, shorted output, enable/disable and realistic TEC wiring. Leave the optional100 pF VC capacitors unpopulated until measurement supports them.

Two22 µF/25 V X7R1210 capacitors bypass each VCC_INT output. ADI's rule for two16 nC gates requires16 µF effective capacitance. Check manufacturer DC-bias/temperature data for the exact ordered capacitors; the combined effective capacitance must remain above that value at5 V. The input and output capacitors likewise require DC-bias review rather than treating printed capacitance as guaranteed in-circuit capacitance.

The gate-drive LDO contribution is calculated at0.221 W at28 V/300 kHz using32 nC total gate charge, before IC quiescent loss. MOSFET switching loss, ringing, diode recovery, copper spreading and board temperature still require measurement. The IRLR024N threshold boundary is an explicit unresolved qualification item; ERC/DRC cannot validate it.

## Input protection review

The PWP package has three IN pins and three OUT pins. All are connected, and all grounded exposed pads have ground vias. R14 remains4.02 kΩ; MODE remains open for latch-off. B_GATE/DRV, reverse-blocking MOSFET, input dividers, IMON conditioning and PGOOD interlock are retained.

TPS26631 intentionally allows a2× overload pulse after PGOOD, unlike TPS26630. Nominal pulse current is approximately8.96 A for25.5 ms with this ILIM resistor, and fast-trip behavior also differs. This exceeds the adapter's continuous rating. It is transient fault behavior, not authorization to operate above4 A. Validate current-limited startup, brownout, repeated overload, fast short, latch-off and manual reset. Record supply collapse/recovery and contact/copper heating; do not infer those results from an ERC pass.

## Calibration and power budget

`tools/calculate.py` records formulas and assumptions in `calculations.json`. The simple30× current formula yields1.947 A at full scale; the electrical table's51 mV nominal reference yields1.986 A. The table's48–54 mV limits plus shunt tolerance alone imply approximately1.851–2.124 A. DAC, divider, temperature and dynamic errors widen that range.

Retain firmware `MEASUREMENTS_VERIFIED=false` and zero `TEC_MAX_CODE` values until the new board is measured. Test one channel first. Actual operating input current remains≤4 A; four12 V/2 A channels need96 W at the loads and approximately4.44 A from24 V at90% efficiency, before auxiliary consumption. They cannot all run at that operating point within the prototype input limit.

## Assembly and mechanics

All controller signal terminals are exposed0.65 mm pitch leads. Thermal pads remain required and use segmented paste openings. Ground vias in exposed pads can wick solder; use a controlled assembly process and inspect the external leads under magnification. Use fresh paste according to its manufacturer's storage/shelf-life instructions and a new revision-M stencil. The original full-board steel stencil and five-IC holder are not compatible with the changed footprints/coordinates.

The board grows18 mm downward. Copper stackup remains four layers and nominal70/35/35/70 µm,1.6 mm finished thickness. Verify enclosure, lower mounting holes and cable reach before fabrication. Existing through-hole parts and connector orientation remain part of the assembly review.
