# Prototype fabrication package — revision M

`rev-m-prototype-gerbers.zip` contains four copper layers, solder mask, silkscreen, paste, outline, plated/non-plated drills and drill maps. It is exported from the routed revision-M PCB. It is not a production approval or a bench-qualified release. Read ../review/STATUS.md and ../review/DESIGN-REVIEW.md before manufacture.

Board: 160 × 128 mm, four layers, nominal 1.6 mm FR-4. Copper: 70 µm outer layers, 35 µm inner layers; preserve the KiCad stackup. Minimum design clearance 0.15 mm; minimum drill 0.25 mm. Confirm the fabricator supports these features with 70 µm copper. Grounded exposed pads require solder. For open thermal/lead-pad vias, agree a suitable plugging/tenting or filling process with the fabricator and inspect solder wicking during assembly. Do not reuse the revision-L stencil.

Paste layers contain segmented exposed-pad apertures. Determine stencil thickness and paste reduction with the assembler; the CAD apertures are not a validated printing process. Start by commissioning a single TEC channel with a current-limited source and the firmware disabled/calibration defaults, then perform the tests in the design review.
