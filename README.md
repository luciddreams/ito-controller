# ITO-controller — condensatiespiegel

KiCad 9-project voor een 100 × 100 mm ITO-glasplaat met snelle elektrische verwarming en vier afzonderlijk geregelde TEC1-12706-koelkanalen. Besturing: Seeed XIAO BLE nRF52840. Voeding: Mean Well GST160A24-R7B, rechtstreeks op J30 (Kycon KPJX-4S).

**Status:** revisie L, bewaakt prototype; nog niet fysiek getest. Maximaal 4 A totale ingangsstroom. Firmware L.1 ondersteunt korte handmatige proeven met één TEC tegelijk; automatische condensregeling is nog niet geïmplementeerd.

## Begin hier

- [Gedetailleerde projectbeschrijving](docs/PROJECTBESCHRIJVING.md): werking per schemablok, thermisch concept, grenzen en open punten.
- [Schema als PDF](ito-controller-schema-rev-l.pdf) en [leeswijzer](SCHEMA-LEZEN.md).
- Open `ito-controller.kicad_pro` in KiCad 9 voor schema, PCB en 3D-inspectie. Houd de mapstructuur intact.
- [Datasheets en BOM-koppeling](datasheets/README.md).
- [Assemblage en bedrading](ASSEMBLY.md), [volledige connectorpinout](connector-pinout.csv), [prototype-inbedrijfstelling](PROTOTYPE-START.md).
- [Firmware, compileren en flashen](firmware/README.md) en [GPIO/I²C-interface](FIRMWARE-INTERFACE.md).

## Projectinhoud

| Pad | Inhoud |
|---|---|
| `ito-controller.kicad_*`, `sheet_*.kicad_sch` | Bewerkbare PCB en 14-blads schema |
| `ITO.kicad_sym`, `ITO_PCB.pretty/`, `3dmodels/` | Lokale symbolen, footprints en modellen |
| `manufacturing/` | Gerbers, boorbestanden, posities en productie-output |
| `BOM-prototype.csv` | Componentposities, fabrikanten en bestelcodes |
| `external-sensor-BOM.csv` | Externe ambientmodules en kabelonderdelen |
| `firmware/` | Broncode, diagnosebuild en hosttests |
| `datasheets/` | Fabrikantdocumenten, bron-URL’s en downloadstatus |
| `docs/` | Uitgebreide beschrijving en documentatie van de import |
| `*-review.json`, `*-parity.json`, `*.rpt` | Ontwerpcontroles en vergelijkingen |
| `commissioning-results.csv` | Nog uit te voeren fysieke tests |
| `history-rev-j/`, `history-rev-k/` | Historie; niet gebruiken als huidige bouwinstructie |

## Productie en verificatie

Lees [FABRICATION.md](FABRICATION.md) vóór bestellen: vier lagen, 160 × 110 mm, 1,6 mm, koper 70/35/35/70 µm en specifieke via-in-pad-eisen. Bouw eerst één prototype.

Bestaande rapporten: ERC 0 meldingen; DRC 0 overtredingen; schema/PCB-pariteit 202 componenten en 638 pinnen. De schematische herschikking heeft de PCB niet gewijzigd. De rapporten en firmwaretests vervangen de fysieke testreeks niet.

## Belangrijke vervolgstappen

1. Exacte NTC’s, fan, koelconstructie en glascoating vastleggen.
2. Onbelaste print inspecteren en hulpvoedingen controleren.
3. Met dummybelastingen meten en firmwarekalibratie invullen.
4. Eén TEC en daarna glas met korte begrensde pulsen kwalificeren.
5. Pas daarna vierkanaalsbedrijf en automatische dauwpuntregeling ontwikkelen.

Voor remote SHT31’s gelden de volledige adapter- en jumperinstructies in [REMOTE-SENSORS.md](REMOTE-SENSORS.md). De RJ45-kabels zijn geen Ethernet/PoE.

## Licenties

Er is nog geen open-sourcelicentie voor eigen hardware/firmware gekozen. Meegeleverde fabrikantdocumenten en modellen behouden hun oorspronkelijke rechten en licenties. Zie [THIRD-PARTY-NOTICES.md](THIRD-PARTY-NOTICES.md).
