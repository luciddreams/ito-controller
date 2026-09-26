# ITO-condensatiespiegel — PCB revisie J

**De elektrische routing is voltooid. Deze revisie is een controleerbaar ontwerp,
geen vrijgegeven productieprint.** Exacte vermogensonderdelen en de schakelende
voedingslayout moeten nog worden gekwalificeerd voordat de print besteld wordt.

Pak de hele ZIP uit en open `ito-controller.kicad_pro` in KiCad 9.
Het pakket bevat de PCB, twaalf schemabladen en lokale symbool- en footprintbibliotheken.

## Uitgevoerde controles

| Controle | Resultaat |
|---|---:|
| Native KiCad ERC: fouten / waarschuwingen | 0 / 0 |
| Native KiCad DRC: overtredingen | 0 |
| Ontbrekende elektrische verbindingen | 0 |
| Schema/PCB: gecontroleerde pinnen | 588 |
| Schema/PCB: afwijkende netaansluitingen | 0 |
| Elektrische componenten / montagegaten | 191 / 4 |
| Baansegmenten / vias | 4168 / 259 |

De rapporten `schematic-erc.rpt`, `routing-drc.rpt`, `net-parity.json` en
`pcb-validation.json` zijn inbegrepen. De DRC gebruikt de meegeleverde
0,15mm-koperafstandregel. Een nulresultaat bewijst geen stroomcapaciteit,
regelstabiliteit, EMC, thermische veiligheid of passende bestelnummers.

## Ontwerp

- Print 160 × 110 mm, vier koperlagen, nominale printdikte 1,6 mm.
- Seeed XIAO BLE nRF52840 op twee vrouwelijke 1×7 headers, steek 2,54 mm.
- 24V-ingang, MOSFET-ingangsbeveiliging en hardware-vrijgaverelais.
- Eén ITO-heateruitgang met stroommeting; vier LT8613-Peltierregelkanalen.
- Instelling van TEC-stroom via DAC; temperatuur-, ventilator- en sensorinterfaces.
- Antennegebied vrij van koper. Massavlakken op In1, boven- en onderzijde.
- Alle vier lagen bevatten nu plaatselijk routes, ook de laag met het massavlak.

Het glas is 100 × 100 × 1,1 mm, met een opgegeven laagweerstand van 8 Ω/□.
Met twee 5mm-busbars is de actieve afstand nominaal 90 mm: circa 7,2 Ω en
80 W elektrisch bij 24 V. Dit is een berekening, geen gemeten heaterweerstand
of goedgekeurde boostinstelling. De vier TEC-kanalen staan uit tijdens ontwasemen.

## Wijzigingen in J

- BIAS van alle vier LT8613-regelaars gevoed vanaf de eigen uitgangscondensator.
- Vier aparte voedingsroutes toegevoegd vóór de stroomshunts.
- Massa-eiland bij kanaal2 met een extra via aan de massavlakken verbonden.
- Schema/PCB-netten, ERC/DRC, elf Kelvin-takken en vier BIAS-takken gecontroleerd.

Zie `BIAS-REVIEW.md` voor de wijziging, verwachte verliesreductie en prototypeproeven.
De eerdere uitgangscapkeuze, tolerantie- en vermogensbudgetberekeningen blijven gelden.

## Open punten en vrijgave

Zie `ENGINEERING-REVIEW.md` voor berekeningen en `OPEN-POINTS.md` voor de resterende
vrijgavecriteria. De selectie van enkele onderdelen sluit hun thermische en
dynamische kwalificatie niet af. Het volledige prototype blijft niet vrijgegeven.
Er zijn geen Gerbers toegevoegd.

## Bestanden

- `pcb-top.svg`, `pcb-bottom.svg`, `plots/`: native KiCad-laagweergaven.
- `BOM-review.csv`: onderdelen en expliciete selectiestatus.
- `connector-pinout.csv`: aansluitvolgorde en netnamen, inclusief XIAO-headers.
- `BUILD-NOTES.md`: voeding, aansluiting en inbedrijfstelling.
- `POWER-REVIEW.md`: vermogensroutecontrole en resterende beperkingen.

## Verificatieaanvulling

Zie `qualification/QUALIFICATION-REVIEW.md`: nieuwe tolerantie- en ingangsbudget-
berekeningen, reproduceerbare scripts en een concreet meetprotocol. Bij 20V vraagt
96W TEC in het gekozen model circa 7,03A. De 12V/2A-instellingen zijn geen harde
limieten. Fysieke kwalificatie is nog niet uitgevoerd; de BIAS-aansluiting is in J gewijzigd, zie BIAS-REVIEW.md.

## Afzonderlijke kandidaat voor de ingangstrap

`input-stage-candidate/` bevat een nieuw, elektrisch gecontroleerd eFuse-schema.
Dit is NIET in de hoofdprint geïntegreerd. De voorlopige4,5A-begrenzing reduceert
het beschikbare systeemvermogen. Lees de integratievoorwaarden in die map voordat
onderdelen of PCB-routes worden gewijzigd.
