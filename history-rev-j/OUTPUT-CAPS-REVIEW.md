# Uitgangscondensatoren — revisie I

## Selectie en fysieke wijziging

C107/C108, C207/C208, C307/C308 en C407/C408 zijn nu KEMET/Yageo
C2220C476M3R2C7186: 47µF ±20%, 25V, X7R, gestapelde KPS-MLCC.
De oude 1210-reservering is vervangen. De fabrikantlandpattern heeft twee
2,08×4,78mm pads met hartafstand 5,38mm; courtyard 7,70×6,00mm.
De behuizing is nominaal 6×5mm, maximaal 5,5mm hoog. Montage volgens KPS-reflowinstructies.
Er is geen nauwkeurig 3D-model meegeleverd; controleer hoogte met echte onderdelen.

Alle acht footprints, schemawaarden en BOM-regels zijn aangepast. Beide caps
per kanaal zijn verplaatst. Twaalf 1,2mm-vermogensverbindingen tussen spoel,
condensatoren en shunt zijn opnieuw gelegd; nieuwe vermogensvias zijn 1,0/0,5mm.
De massa-aansluitingen zijn solid verbonden met de kopervlakken. Deze geometrie
bewijst nog geen laaginductieve retourlus; de schakelende layoutreview blijft nodig.

## Effectieve capaciteit en rimpel — berekening, geen meting

De typische fabrikantcurve geeft bij 12V ongeveer 13% capaciteitsverlies.
Twee parallelle onderdelen leveren nominaal 94µF en typisch circa 81,8µF bij 12V.
Die grafiek is geen gegarandeerde minimumwaarde. Een extra ongunstig rekenscenario
met 0,80 tolerantie, 0,85 temperatuurfactor en 0,94 verouderingsfactor geeft
94×0,87×0,80×0,85×0,94 = 52,3µF. Voor gevoeligheidsanalyse gebruiken we 50µF.
De extra 6% veroudering is een expliciete aanname; dit product van factoren is
geen gekwalificeerde ondergrens en behandelt afhankelijkheden niet.

Voor een ideale buck bij 28V in, 12V uit, L=8µH (−20% op 10µH), 500kHz:
ΔIL = (28−12)×(12/28)/(8µH×500kHz) = 1,714A piek-piek.
Ideale capacitieve rimpel ΔV = ΔIL/(8×f×C):

| Capaciteit per kanaal | Berekende capacitieve rimpel |
|---|---:|
| 94µF nominaal | 4,56mV p-p |
| 81,8µF typisch bij 12V | 5,24mV p-p |
| 50µF rekenscenario | 8,57mV p-p |

Deze waarden laten ESR, ESL, kabels, burstbedrijf en regeldynamiek buiten beschouwing.
Ideale condensator-RMS-stroom is 0,495A totaal, circa 0,247A per onderdeel bij
perfect gelijke verdeling. Controleer werkelijke opwarming en stroomverdeling.
Bij een hypothetische 2A-laststap zonder regelreactie geeft 50µF gedurende 10µs
al 0,4V verandering (100µs: 4V). Dit illustreert gevoeligheid, geen voorspelde overshoot.
Nominaal zit bij 12V 6,77mJ in beide caps samen; uitschakelen verwijdert die energie niet.

## Nog te sluiten

Onderdeelkeuze en padmaten zijn vastgelegd. Regelstabiliteit is niet bewezen.
Controleer effectieve capaciteit over bias/temperatuur/veroudering, stroomluscompensatie,
CC/CV-overgang, staprespons, kortsluiting, open belasting en uitschakelgedrag op één kanaal.
Herhaal relevante proeven met vier kanalen en werkelijke TEC-kabels. Beoordeel daarna
of meer capaciteit of compensatieaanpassing nodig is. De DC-bias van de ingangscaps
blijft afzonderlijk open. Deze revisie verandert de inschakel-SOA niet.

## Fabrikantbronnen

- Onderdeel en typische curves: https://search.kemet.com/download/specsheet/C2220C476M3R2C7186
- KPS-serie, landpattern en assemblage: https://content.kemet.com/datasheets/KEM_C1129_KPS_X7R.pdf

Geraadpleegd 23 september 2026. Typische curves zijn voor ontwerpanalyse;
voorraad en prijs zijn niet vastgesteld.
