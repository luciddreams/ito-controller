# Placement correction — 24 September 2026

## Changes

- C5 pin 1 at (12.00,101.75) mm, body center (14.50,101.75) mm. The radial capacitor clears Q2, F1 and Q5. Added a 6.50 mm radius courtyard.
- Resistor group beside XIAO: columns at 4.00 mm spacing, rows at 2.50 mm. Horizontal component orientation retained. Seventeen staggered resistors aligned without changing values, nets or references.
- Ground planes joined using additional vias; R54 ground pad directly connected and J24 pin-2 thermal spokes set to 45°.
- Connections rerouted and obsolete track fragments removed. C5 has a new 0.80 mm power branch; local eFuse decoupling remains in place.
- Gerbers, drills, SMT positions, layer plots, STEP and KiCad renders updated.

## Checks

Schematic/PCB net and component comparisons found no differences. KiCad DRC: zero violations, unconnected pads and footprint mismatches. STEP intersection checks found no unwanted overlap above 0.05 mm³. Model simplifications and assembly assumptions are described in `3D-INSPECTION.md`; this is not a tolerance simulation or physical qualification.

The previous fabrication hold caused by C5 overlap was removed. Use the corrected revision-L package, assemble one prototype first, and follow commissioning instructions.

Exact before/after positions: `3dmodels/placement-changes.json`.
