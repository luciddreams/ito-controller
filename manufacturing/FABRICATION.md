# Fabrication — ITO revision L prototype

Upload only `manufacturing/ito-l-fabrication.zip` to the PCB manufacturer. The full project archive is for design and assembly. Order a small run, for example five bare boards, and assemble one first.

| Property | Order specification |
|---|---|
| Size / thickness | 160 × 110 mm / 1.60 mm ±10% |
| Material | FR-4, Tg ≥150°C |
| Copper layers | 4: F.Cu / In1.Cu / In2.Cu / B.Cu |
| Copper weight | Nominal finished outer copper 70 µm, inner 35 µm; do not silently reduce outer layers to 1 oz |
| Finish | ENIG |
| Mask / legend | Green solder mask, white silkscreen |
| Minimum track / clearance | 0.15 / 0.15 mm; manufacturer must support this with 2 oz outer copper |
| Smallest via | 0.25 mm finished hole, minimum copper diameter 0.55 mm |
| Via-in-pad | Epoxy filled and copper capped, IPC-4761 type VII, for QFN exposed-pad vias; see via-in-pad.csv |
| Drill files | Separate PTH and NPTH, metric units |
| Tests | Electrical net test and AOI; no controlled-impedance requirement |
| Stackup | Manufacturer's symmetric standard four-layer stack, 1.6 mm total and copper as specified above |

In1.Cu contains ground and some tracks; it is not an uninterrupted plane. The antenna area must stay clear on every copper layer. Gerbers were exported with filled zones; do not refill them using a different rule set.

Have normal DFM confirm the 2 oz / 0.15 mm combination and filled/capped vias. If a low-cost service does not support these requirements, select the appropriate process or another manufacturer. Do not change clearances or remove vias without design review.

SMT stencil: 100 µm, using original F.Paste data. U1 has segmented paste apertures. Filled/capped exposed-pad vias prevent solder loss. QFNs and stacked output capacitors require reflow and inspection. Placement angles follow KiCad conventions; the assembler must verify rotation against pin-1 marks and the assembly drawing.

CAD/export checks establish connectivity and fabrication geometry, not operation of assembled hardware.

## J30 R7B plated slots

J30 has four **0.60 × 2.70 mm plated slots**, plus three unplated mounting holes: two Ø1.70 mm and one 2.20 × 1.00 mm slot. Dimensions are finished sizes from the Kycon drawing. Preserve Excellon routing data; do not replace slots with round holes. Have DFM confirm 0.60 mm plated-slot capability and finished tolerances.
