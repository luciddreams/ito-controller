# ITO Controller — Condensation Mirror

KiCad 9 project for a 100 × 100 mm ITO glass plate with resistive heating and four independently regulated TEC1-12706 cooling channels. Controller: Seeed XIAO BLE nRF52840. Supply: Mean Well GST160A24-R7B, connected directly to J30 (Kycon KPJX-4S).

**Status:** revision L, supervised prototype; no assembled hardware has been tested. Maximum actual total input current: 4 A. Firmware L.1 supports short manual tests with one TEC at a time. Automatic condensation/dew-point control is not implemented.

## Start here

- [Detailed project description](docs/PROJECT-DESCRIPTION.md): circuit operation, thermal concept, operating limits and outstanding work.
- [Schematic PDF](ito-controller-schema-rev-l.pdf) and [schematic reading guide](SCHEMATIC-GUIDE.md).
- Open `ito-controller.kicad_pro` in KiCad 9 for the schematic, PCB and 3D viewer. Keep the directory structure intact.
- [Datasheets and BOM coverage](datasheets/README.md).
- [Purchasing BOM and order tracker](procurement/README.md): ordered/delivered checkboxes, suppliers, prices and quantities.
- [Assembly and wiring](ASSEMBLY.md), [connector pinout](connector-pinout.csv) and [prototype commissioning](PROTOTYPE-START.md).
- [Firmware build, flashing and calibration](firmware/README.md) and [GPIO/I²C contract](FIRMWARE-INTERFACE.md).

## Repository contents

| Path | Contents |
|---|---|
| `ito-controller.kicad_*`, `sheet_*.kicad_sch` | Editable PCB and 14-page schematic |
| `ITO.kicad_sym`, `ITO_PCB.pretty/`, `3dmodels/` | Local symbols, footprints and models |
| `manufacturing/` | Gerbers, drills, placement files and fabrication output |
| `BOM-prototype.csv` | Component references, manufacturers and orderable part numbers |
| `external-sensor-BOM.csv` | External ambient modules and cable parts |
| `procurement/` | Browser purchasing tracker, Excel alternative and instructions |
| `firmware/` | Source code, diagnostic build and host tests |
| `datasheets/` | Manufacturer documents, source URLs and download status |
| `docs/` | Detailed description and import information |
| `*-review.json`, `*-parity.json`, `*.rpt` | Design checks and comparison reports |
| `commissioning-results.csv` | Physical tests awaiting execution |
| `history-rev-j/`, `history-rev-k/` | Historical records; not current construction instructions |

## Fabrication and verification

Read [FABRICATION.md](FABRICATION.md) before ordering: four layers, 160 × 110 mm, 1.6 mm thick, 70/35/35/70 µm copper, with specified via-in-pad processing. Assemble one prototype first.

Existing reports record zero ERC/DRC violations and schematic/PCB parity for 202 components and 638 pins. The readable-schematic revision preserved the PCB. Design checks and firmware host tests do not replace physical commissioning.

## Next engineering steps

1. Select the exact NTC probes, fan, cooling assembly and glass coating.
2. Inspect the unloaded board and verify auxiliary supplies.
3. Measure with dummy loads and enter firmware calibration data.
4. Qualify one TEC, then the glass, using short limited-power pulses.
5. Develop four-channel operation and automatic dew-point control only after qualification.

Remote SHT31 sensors require the complete adapter and jumper instructions in [REMOTE-SENSORS.md](REMOTE-SENSORS.md). Their RJ45 links are not Ethernet or PoE.

## Language and licensing

English is the working language for current documentation and schematic annotations. Original historical engineering records are retained in their original language for traceability; see [historical records](docs/HISTORICAL-RECORDS.md). Manufacturer PDFs retain the language supplied by their authors. Legacy component Value fields and net identifiers remain stable for CAD/BOM compatibility.

No open-source license has been selected for the project's own hardware or firmware. Third-party documents and models retain their original rights and licenses; see [THIRD-PARTY-NOTICES.md](THIRD-PARTY-NOTICES.md).
