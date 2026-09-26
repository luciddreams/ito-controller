# R7B PCB input and connector correction — 24 September 2026

J30 provides a direct Mean Well GST160A24-R7B input using the locking four-pin through-hole Kycon KPJX-4S. Both positive contacts (1/4) and both returns (2/3) are connected. J3 remains an alternative input; never connect two supplies.

J30 faces outward at the lower edge. Footprint origin/front center is (82, 114) mm, rotation 180°. Its nose extends 4 mm beyond the 160 × 110 mm PCB. Allow enclosure clearance for plug and latch. The included STEP is a simplified dimensional model, not manufacturer CAD; verify mating and latch force with the actual component.

Kycon drawing A13 is a bottom view. With opening towards −Y, local footprint pins are: 1=(+2.90,14.65), 2=(−2.90,14.65), 3=(+2.50,11.00), 4=(−2.50,11.00) mm. Contact holes are four 0.60 × 2.70 mm plated slots; see FABRICATION.md.

J3 and J11–J14 were rotated 180° about the midpoint between their pins, preserving pin numbering/nets, repairing routing and adding +/− markings. J11–J14 open towards the upper edge; J3 towards free space above its terminal. C5 and the aligned resistor group remain in their corrected positions.

Schematic, BOM, pinout, local libraries and 3D model were updated. Firmware remains L.1. Limits remain 4 A total and 2 A measured per TEC channel; this change does not qualify higher power.

## Sources

- [Kycon KPJX-4S A13](https://www.kycon.com/Pub_Eng_Draw/KPJX-4S.pdf)
- [Mean Well GST160A](https://www.meanwell.com/Upload/PDF/GST160A/GST160A-SPEC.PDF)
- [Mean Well adapter selection](https://www.meanwell.com/upload/pdf/DC_plug.pdf)

Recorded validation: ERC/DRC zero violations, unconnected pads and footprint errors; 202 components and 638 pins matched; one connected ground network; 11 Kelvin branches and four BIAS branches preserved. J30 input routing is at least 2.0 mm on 70 µm outer copper without added power vias. J3–F1 also uses 2.0 mm tracks; TEC outputs remain at least 1.2 mm. Totals: 7,442 track segments and 323 vias. Independent Gerber/Excellon readback: 423 PTH objects and seven NPTH objects. Physical tests are still required.
