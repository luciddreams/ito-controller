# Geometrische vermogensroutecontrole — revisie I

De controle zoekt aaneengesloten koperpaden tussen pads op hetzelfde net.
Zij rekent niet aan via-stroom, gedeelde stroom door parallelle paden,
planes, temperatuur, regelstabiliteit of transientgedrag.

| Traject | Gevonden baanbreedte / aandachtspunt |
|---|---|
| J3 ingang → F1 | Continu 2mm-pad aanwezig |
| F1 → Q2 drain | Brede route, lokale vernauwing circa 1,905 mm |
| Q2 source → Q3 source | 4 mm |
| Q3 drain → beveiligde bus | Lokale vernauwing circa 1,905 mm |
| Belastingsbus → zekeringen | Brede routes, lokale vernauwing circa 1,700 mm |
| Heaterzekering → shunt → heaterconnector | 2 mm |
| Heaterretour → MOSFET | 2 mm, binnenlaag |
| TEC-inductor → shunt → uitgangsconnector | 1,2 mm hoofdpaden |
| LT8613 ISP/ISN en INA226 meetaftakkingen | Nieuwe 0,15mm-meetroutes; verbinding met vermogensnet pas op eigen shuntpad geometrisch gecontroleerd |
| Alle vier VIN-cellen | Herlegd: 1,2mm bulk/feedroutes, 0,8mm lokale cap-aanvoer, 0,6mm cap-IC en korte QFN-pinverbindingen; nieuwe vermogensvias 1,0/0,5mm |
| Voeding ventilatorzekering F3 | Extra 0,8mm-route toegevoegd; oorspronkelijke aftakking behouden; definitieve fanstroom/zekering nog onbekend |

De afzonderlijke VIN- en SW-pinnen van de QFN zijn plaatselijk met korte dunne
segmenten gekoppeld. Een minimum per net is daarom niet hetzelfde als de breedte
van het hoofdvermogenspad. Het JSON-rapport onderscheidt de padparen.

De 0,25mm-boorgaten van sommige vias zijn geen bewijs dat deze de gewenste
piek-/continuestroom aankunnen. Leg plating en koperopbouw vast en controleer
de temperatuurstijging. Er is geen IPC-stroomcapaciteitsberekening uitgevoerd.

De massavlakken zijn opnieuw gevuld en elektrisch verbonden volgens native DRC.
De verdeling van de retourstroom en onderbrekingen door signaalroutes zijn nog
niet met een veld- of impedantiemodel onderzocht.

De schakelende TEC-regelaars moeten vóór fabricage een aparte layoutreview
krijgen: VIN-condensator/IC/GND-lus, SW-oppervlak, bootstrap, lokale ontkoppeling,
Kelvin-paden en feedback. Ook met nul DRC-fouten is deze stap nog noodzakelijk.

## Revisie I

De grotere KPS-uitgangscaps hebben nieuwe 1,2mm-vermogensroutes.
De elf afzonderlijke meettakken zijn opnieuw geometrisch gecontroleerd.
Zie de opnieuw gegenereerde JSON-rapporten voor deze revisie.

## Aanvulling revisie J

De BIAS-voeding is naar de eigen uitgangsrail verplaatst. Zie BIAS-REVIEW.md
voor topologie, rekenvoorbeelden en de nog vereiste fysieke verificatie.
