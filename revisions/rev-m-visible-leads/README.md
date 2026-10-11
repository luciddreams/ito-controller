# Revision M — visible-lead prototype

This independent KiCad project replaces the five QFN ICs in revision L. Signal pins on U1 and U11–U14 are exposed gull-wing leads. Their grounded thermal pads still require soldering. The original manufactured design at the repository root is unchanged.

**Engineering prototype, not a bench-qualified production release.** See `review/STATUS.md` for the actual checks and outstanding release gates. Do not use revision-L Gerbers, stencil, component placement or calibration for this board.

## Main changes

| Circuit | Revision L | Revision M |
|---|---|---|
| U1 input eFuse | TPS26630RGER, QFN24 | TPS26631PWPR, HTSSOP20 + ground pad |
| U11–U14 TEC control | LT8613EUDE, QFN28 | LT3741EFE-1#PBF, TSSOP20 + ground pad |
| TEC switches | Inside LT8613 | Two IRLR024NTRPBF DPAK MOSFETs per channel |
| L1–L4 | 10 µH IHLP6767 | 33 µH Bourns SRP1265A-330M |
| Switching frequency | Nominal 500 kHz | Nominal 300 kHz |
| TP11–TP14 monitoring | LT8613 IMON | INA180A1IDBVR, SOT23-5, 0.5 V/A nominal |
| Board | 160 × 110 mm, four layers | 160 × 128 mm, four layers |

The upper mounting holes and four TEC connector locations are retained. Lower circuitry, lower mounting holes and the antenna keepout move down 18 mm. This revision therefore requires a mechanical fit check. The lower schematic functions, GPIO assignments, I²C addresses, relay/thermostat chain and heater interlock remain unchanged.

Open **`ito-controller.kicad_pro` in this directory** with KiCad 9. The symbol and footprint libraries are local. Existing 3D models are referenced from `../../3dmodels`; new package footprints currently have no 3D models. Electrical and fabrication geometry does not depend on those models.

## Electrical differences that matter

- TPS26631 is **not fault-identical** to TPS26630. After PGOOD, it permits approximately twice the programmed current for nominally 25.5 ms. With 4.02 kΩ, nominal sustained limit is approximately 4.48 A and the temporary limit approximately 8.96 A. Actual operating input current must still remain at or below 4 A. Verify the adapter, fuse, reset and interlock response with the new eFuse.
- LT3741-1 controls **average inductor current** and does not intentionally sink current. The 25 mΩ shunt is now before the output capacitors. TP monitoring therefore includes inductor ripple and capacitor charging current. Use an external meter/current probe in the TEC lead for load-current calibration.
- R42/R44/R46/R48 change to 4.02 kΩ. The lower 10 kΩ resistors and buffers remain. Full DAC code gives approximately 1.460 V at CTRL1. Expected nominal full-scale current is about 1.99 A using the 51 mV reference specification; part tolerances can produce over 2 A. This is **not a guaranteed 2 A limiter**. Calibrate each channel and use conservative code limits.
- The nominal voltage ceiling is 11.94 V. The 22.1 kΩ / 6.8 nF compensation network is a calculated starting point. C109/C209/C309/C409 are optional 100 pF compensation footprints and **DNP** initially.
- IRLR024N has a maximum 2 V threshold at 25 °C, at the boundary of ADI's recommendation of less than 2 V. Startup/recovery and cold operation are explicit qualification gates. The footprint permits reviewing another pin-compatible DPAK part if this gate fails; substitution is not automatic.

The diagnostic firmware at `../../firmware` writes raw DAC codes and retains the same GPIO/I²C interface. Use its default disabled calibration state. **Do not carry over measured TEC code limits from revision L.** This revision does not turn that firmware into an automatic or unattended controller.

## Files

- Native schematics, PCB and local libraries: editable design.
- `design.json`: component pin/net and ordering data.
- `BOM-prototype.csv`: individual references; Fit column identifies the four DNP options.
- `review/`: calculation results, clean ERC/DRC/parity reports and prototype test gates.
- `fabrication/`: prototype Gerber/drill archive and manufacturing notes.
- `tools/`: reproducible design transformation, calculation and audit helpers. Generation starts from the unchanged revision-L project; final routed PCB is the authoritative layout.

Changing a package or component value can affect compensation, protection or assembly. Read `review/DESIGN-REVIEW.md` before ordering alternatives.
