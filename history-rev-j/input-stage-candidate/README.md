# Kandidaat voor vervanging van de ingangstrap

**Dit is een afzonderlijk elektrisch voorstel. Het is NIET verwerkt in PCB revisie J,
niet gerouteerd en niet vrijgegeven voor fabricage.** De hoofdprint en haar bestaande
validatierapporten blijven ongewijzigd. Open hier `input-stage-candidate.kicad_sch`.
Het schema gebruikt netlabels en functionele symbolen; de lokale bibliotheek is bijgevoegd.

## Waarom deze wijziging

De bestaande LTC4365 stuurt twee externe MOSFETs en vertraagt het inschakelen,
maar regelt geen overstroom. De toegestane inschakelbelasting van die MOSFETs
is daarom nog onvoldoende onderbouwd. Het voorstel gebruikt TPS26630RGER voor
actieve stroombegrenzing en het opladen van C5. Een externe IRL540NPBF krijgt
alleen de functie van blokkeertransistor. BSS138 trekt zijn gate snel omlaag
bij de bedoelde ompool-/terugstroomcondities. De energie van het laden verdwijnt
niet: zij wordt nu hoofdzakelijk in de interne eFuse-transistor verwerkt.

De TPS26630 is gekozen vanwege zijn normale 1×-stroombegrenzing. De varianten
met 2×-pulsstroom mogen niet zonder nieuwe berekening worden ingevuld. MODE blijft
bewust open voor vergrendelde uitschakeling na overbelasting; SW90 trekt SHDN
momentaan laag voor reset. UV/OV-fouten hebben hun eigen herstelgedrag: 'latch-off'
betekent hier dus niet dat elke mogelijke fout uitsluitend handmatig herstelt.

## Voorlopige dimensionering

| Instelling | Voorstel | Berekende betekenis |
|---|---|---|
| Stroomweerstand R90 | 4,02kΩ / 0,1% | Circa4,5A nominaal; circa4,18–4,82A met tabelgrenzen en weerstandstolerantie |
| Rampcondensator C91 | 100nF / 5%, stabiel bij circa1V | Circa58ms bij28V; modelhoeken circa46–70ms |
| Directe uitgangscapaciteit | Maximaal600µF als scenario | Circa0,288A nominale laadstroom; snelle rekenhoek0,365A |
| UV-deler | 156kΩ / 10kΩ, beide0,1% | Nominaal19,92V; stijgende drempelscreening19,46–20,38V |
| OV-deler | 223kΩ / 10kΩ, beide0,1% | Nominaal27,96V; stijgende drempelscreening27,31–28,61V |
| PGOOD-deler | 150kΩ / 10kΩ, beide0,1% | Nominaal19,2V |
| Stroommonitor R99 | 10kΩ / 0,1% | Nominaal circa0,279V/A |

Dit zijn geen nieuwe systeemratings. Actieve stroombegrenzing is geen absolute
begrenzing van elke kortsluitpiek. De rampberekening laat actieve lasten, interne
startvertraging en ingrijpen van de thermische regeling buiten beschouwing.
600µF moet met de werkelijke fan-/hulpvoedingscapaciteit worden gecontroleerd.
De TPS-regeling kan de start verlengen. De externe blokkeertransistor en bron
blijven afzonderlijk op transiënten te controleren.

**Gevolg voor het vermogen:** reserveer voorlopig maximaal4,0A continue bronstroom
onder deze4,5A-beveiligingsinstelling. Bij24V geeft het rekenmodel met15,125W
hulpbelasting,85% TEC-rendement en aangenomen serieweerstanden ongeveer66W totaal
TEC-vermogen. Viermaal12V/2A=96W is daarmee geen toegestane combinatie.
Ook80W heater plus een volledige12W fanbelasting past niet met ruime marge binnen
het4,0A-doel. Heater-PWM en hulpbelasting moeten samen worden begrensd.

De lagere stroomkeuze is voorlopig: de uiteindelijke grens moet passen bij de
voeding, TEC-modellen en koeling. Een weerstand voor circa6A kiezen geeft vanwege
toleranties geen harde6A-grens en vraagt een nieuw thermisch/stroombudget.

## Integratie in de hoofdprint

| Hoofdprint revisie J | Voorgestelde wijziging |
|---|---|
| U1 LTC4365 en gate-rampnetwerk | Vervangen door U90 en zijn UV/OV/dVdT/ILIM-netwerk |
| Q2/Q3 | Vervangen door één blokkeer-FET met andere netoriëntatie en Q91; geen directe pin-voor-pin-vervanging |
| F1 / D1 / C5 | Functies behouden; F90/D90/C92 stellen deze bestaande onderdelen voor, niet drie extra parallelle onderdelen |
| +24_PROT | Aansluiten op VPROT achter de eFuse |
| PROT_FAULT_N | Koppelen aan FAULT_N na controle van logicaniveau en firmware |
| Vrijgave relais/TEC/heater | Aan PGOOD en de bestaande veilige MCU-opstartvolgorde koppelen |
| Totale stroommeting | IMON naar een geschikt ADC-kanaal met dimensioneerde ingangsbescherming/filtering |

M1 moet tijdens de eFuse-ramp kunnen opstarten, zodat de3V3-logica beschikbaar
komt. PGOOD mag niet de eigen hulpvoeding afschakelen en daarmee een startblokkade
veroorzaken. Houd hoofdlasten uit tot de spanning stabiel en PGOOD geldig is.
De integratie van PGOOD/IMON is nog niet getekend in de hoofdprint en niet getest.

## Controles en nog uit te voeren werk

Native KiCad ERC:0 fouten,0 waarschuwingen. Alle65 bewust verbonden aansluitingen
zijn tegen de componentenlijst gecontroleerd; ontbrekende aansluitingen:0.
Dit controleert het schema, geen thermische werking of de fysieke padmaten.

Nog nodig: exacte BSS138-leverancier en C91, VQFN-landpattern en thermische vias,
plaatsing/routing, TVS/broncoördinatie, definitieve stroomgrens, PGOOD-interlock,
IMON-ADC-interface en prototypeproeven. De blokkeer-FET staat anders aangesloten
dan de oude lineaire ingangstrap; de oude koperbanen mogen niet worden hergebruikt
zonder netcontrole. Geen Gerbers of bestellijst voor deze kandidaat inbegrepen.

## Bestanden

- `candidate-components.json`: waarden, pinfuncties, netten en selectiestatus.
- `calculated-corners.json` / `calculate.py`: reproduceerbare eigen berekeningen.
- `netlist.xml`, `net-check.json`, `erc.rpt`: verificatieresultaten.
- `preview/`: schaalbare schemaweergave en PNG.

Bron: TI TPS2663 Rev.G, pinnen, elektrische tabellen en toepassingsschema9-2:
https://www.ti.com/lit/ds/symlink/tps2663.pdf

De IRL540NPBF is de bestaande geselecteerde MOSFET; zijn continue thermische
kwalificatie blijft nodig. Geen voorraad- of prijsclaim en geen hardwaremeting verricht.
