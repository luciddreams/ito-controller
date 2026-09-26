# 3D inspection — ITO controller L

J30 is the direct R7B socket. J3 and J11–J14 were rotated 180°; see R7B-UPDATE.md. J30 uses a simplified STEP envelope based on Kycon A13, including front overhang and mounting pins.

## Placement correction: 24 September 2026

C5 pin 1 moved from (15.60, 79.10) to (12.00, 101.75) mm; body center is (14.50, 101.75) mm. Seventeen resistors beside the XIAO were aligned on a regular grid; see PLACEMENT-UPDATE.md.

Earlier C5/Q2, C5/F1 and C5/Q5 model collisions were resolved. Repeated STEP intersection checks found no unwanted overlap above the 0.05 mm³ reporting threshold. The placement-related fabrication hold was removed; prototype build, calibration and physical test requirements still apply.

## Opening the model

1. Keep the complete project, including `3dmodels` and `ITO_PCB.pretty`.
2. Open `ito-controller.kicad_pro` in KiCad 9, then PCB Editor.
3. Choose **View → 3D Viewer** (Alt+3), enabling both THT and SMD models.
4. Models use `${KIPRJMOD}/3dmodels/...step`; no system model-library installation is required.
5. J1 has two models: its socket and a simplified XIAO assembly. Disable the XIAO model in footprint properties → 3D Models to inspect beneath it. This model belongs only to placed J1, not the generic socket library.

`3d-review/isometric.png`, `top.png` and `bottom.png` are native KiCad renders. `3d-review/ito-controller-rev-l.step` includes components for mechanical CAD inspection.

## Coverage and accuracy

All **202 electrical component positions** have local STEP models. Four mounting holes and three fiducials are PCB geometry and need no separate component models. The plugged-in XIAO is also represented by a simplified assembly model.

Standard packages come from the official KiCad library. They are package models, not certified models of each exact MPN. Origins, pin 1 and rotation follow footprint conventions and were visually reviewed. Actual parts take precedence; always verify polarity against schematic, BOM and datasheet.

| Simplified model | Assumptions / limitations |
|---|---|
| C5 | Ø12.5 mm, center 2.5 mm from pin 1, 5 mm pin pitch, 22 mm height reservation. No exact base/vent detail; replaces the undersized generic model. |
| K1 G2RL-1A-E | 29 × 12.7 × 15.7 mm envelope, six pins on existing grid; no internal details. Based on Omron G2RL drawing. |
| L1–L4 IHLP-6767 | Simplified 17.15 × 17.15 mm body, approximately 12.55 mm model height. Family shape; exact GZEK100M11 version/tolerances need physical confirmation. |
| KPS capacitors | Nominal 6 × 5 × 5 mm double stack; referenced table maxima are 6.5 × 5.5 × 5.5 mm. Reserve maxima plus clearance. |
| Littelfuse 451 | Simplified 6.1 × 2.69 × 2.69 mm body/end caps, not manufacturer CAD. |
| J1/J2 sockets | 2.54 mm pitch, 8.51 mm family height, simplified holes/pins. |
| XIAO | Orange assembly envelope, PCB 17.8 × 21 mm. Assumes 8.51 mm socket + 2.54 mm male-header spacer + 1 mm PCB. USB/component volumes approximate; actual height depends on headers/insertion depth. |
| WSL shunts | Generic KiCad 2512 body, not exact Vishay terminal geometry. |

Wiring, plugged-in connectors, cable bend radii, screwdriver access, external sensor modules and optional heatsinks are not modeled. No render is a final mechanical release.

## Recorded checks

- C5 and 17 resistors moved; routing repaired. Nets, values and part selections preserved. C5 now has a physical courtyard.
- KiCad DRC: zero violations, unconnected pads and footprint mismatches.
- All referenced STEP files exist and were loaded.
- Solid intersections rechecked; no unwanted collisions reported. Intended XIAO/socket mating excluded. This is not a tolerance simulation or complete clearance analysis.
- Gerbers, drills, placements, STEP and renders regenerated from the corrected PCB. `manufacturing/export-validation.json` records the source hash.

## Provenance and reuse

KiCad: https://github.com/KiCad/kicad-packages3D — per-file source and SHA-256 in `3dmodels/sources.json`. License/attribution: `3dmodels/KICAD-LICENSE.md`, CC-BY-SA 4.0 with the KiCad design exception.

Custom simplified models were generated for this project, reproducible with `3dmodels/generate-custom-models.py` and CadQuery. They are not manufacturer approved.

Mechanical references:

- Omron G2RL datasheet, G2RL-1A-E dimensions.
- KEMET/Yageo KPS X7R C1129, 04/26, page 3, double 2220.
- Seeed: https://wiki.seeedstudio.com/XIAO_BLE/ — 21 × 17.8 mm module.
- Samtec: https://suddendocs.samtec.com/prints/ssw-1xx-xx-xxx-x-xx-xxx-xx-mkt.pdf
- Vishay: https://www.vishay.com/docs/34310/ihlp6767gz-1a.pdf — family approximation, not the exact ordered MPN.
