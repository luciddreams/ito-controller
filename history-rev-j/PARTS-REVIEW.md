# Onderdelenreview — revisie I

## Nieuw geselecteerd

| Posities | Bestelnummer | Ontwerpwaarde |
|---|---|---|
| RS1–RS4 | Vishay WSL2512R0250FEA | 25mΩ, 1%, 1W |
| RSH1 | Vishay WSL2512R0100FEA | 10mΩ, 1%, 1W |
| F1 | Littelfuse 0451010.MRL | 10A |
| F2 | Littelfuse 0451005.MRL | 5A |
| F3 | Littelfuse 0451001.MRL | 1A |
| F11–F14 | Littelfuse 045102.5MRL | 2,5A, ingang van TEC-regelaar |
| C101/C102, C201/C202, C301/C302, C401/C402 | Murata GRM32ER71H106KA12L | 10µF/50V/X7R/10%, 1210 |
| C5 | Panasonic EEUFR1H471 | 470µF/50V/20% |

Selectie betekent dat een concreet onderdeel aan de positie is gekoppeld;
het is geen verklaring dat foutbedrijf, EMC en thermiek al gekwalificeerd zijn.

## Shunts: fabrikantlandpattern en eigen berekeningen

WSL2512 voor deze weerstandswaarden: twee pads 1,65×3,68mm, binnenafstand4,06mm,
padcentra op ±2,855mm. Het generieke 2512-pad is vervangen; drie geraakte
vermogensbanen zijn herlegd. Nominale rating1W geldt bij70°C; boven70°C deraten.
TCR van het gehele onderdeel:75ppm/K. Dit zijn tweepolige shunts; absolute
meetnauwkeurigheid blijft afhankelijk van soldeer- en aansluitgeometrie.

Eigen vermogensberekening:
- TEC bij2A: 2²×0,025 =0,100W. Zelfs met1,714A p-p driehoekrimpel door de shunt:
  (2²+1,714²/12)×0,025 ≈0,106W. De werkelijke shunt zit na de outputcaps.
- Heater bij3,333A RMS: 3,333²×0,010 ≈0,111W.
- Bij6A RMS door heater-shunt:0,360W; dit geeft geen toestemming voor6A door het glas.
- TCR-screening over60K:75ppm/K×60K=0,45%. Samen met1% begintolerantie is alleen
  de resistorbijdrage maximaal circa1,45% via conservatieve lineaire optelling.
  IC-gain, offset, referentie, DAC en thermische EMK komen erbij; kalibratie blijft nodig.

## Zekeringen: eigen stroommarges en grenzen

451-serie125VDC; afschakelvermogen bij32VDC is300A voor de gekozen1/2,5/5A-types
en400A voor10A. Landpattern: pads1,96×3,15mm, gap2,95mm, centra±2,455mm.
De fabrikant vraagt25% derating voor continugebruik, plus temperatuurcorrectie.
Voor onderstaande eigen screening is aanvullend een conservatieve factor0,90
gebruikt; controleer deze tegen de werkelijk gemeten zekeringtemperatuur.

| Tak | Voorlopige belasting | Screening: rating×0,75×0,90 |
|---|---:|---:|
| Hoofdvoeding | 6A ingang als ontwerpbudget | 6,75A |
| Heater | 3,33A RMS bij80W en7,2Ω | 3,375A |
| Ventilator | 0,5A continu aangenomen | 0,675A |
| Iedere TEC-ingang | 24W/(20V×0,85)=1,412A gemiddeld | 1,688A |

De heatermarge is klein bij deze conservatieve temperatuurfactor. Werkelijke
ITO-weerstand en zekeringtemperatuur bepalen of5A passend blijft. Bij28V en
100% duty zou 7,2Ω circa3,89A trekken; dat valt buiten deze screening.
De TEC-zekering ziet ingangsstroom, niet rechtstreeks2A uitgangsstroom.
De nieuwe2,5A-keuze vergroot de marge; hij verandert de2A-regelinstelling niet.
Input-RMS-rimpel kan hoger zijn dan het gemiddelde en moet worden gemeten.

Een zekering is geen precieze stroomregelaar. Bij een bron die op6A begrenst kan
F1=10A blijven geleiden bij kortsluiting. De stroombegrenzing van de bron, hardware-
afschakeling, bedrading en zekeringen moeten samen worden gekwalificeerd.
Nominale smelt-I²t bewijst geen selectiviteit of bescherming van halfgeleiders.
Inschakelpulsen, hoofdrelais sluiten en fanstart moeten worden gemeten en getoetst.

## Condensatoren

C5 past bij het gereserveerde12,5mm-huis en5mm-pitch; hoogte20mm nominaal.
De opgegeven2,26Arms geldt bij100kHz; de fabrikant geeft frequentiecorrectie voor
andere frequenties. De heater-PWM kan de buffer zwaar belasten. Als de voeding
alleen gemiddelde stroom levert is de ideale capacitieve RMS-stroom bij3,33A
lastpuls en50% duty circa3,33×sqrt(0,5×0,5)=1,67A. De werkelijke stroomverdeling
hangt af van bronimpedantie, kabels en PWM-frequentie. Dit is nog geen ripplevrijgave.

De twee10µF/50V-ingangscaps per TEC behouden hun waarde en footprint. De gekozen
MPN is vastgelegd, maar effectieve capaciteit bij24–28V en temperatuur is nog
niet onderbouwd met een DC-biascurve. Nominale20µF is niet hetzelfde als20µF in bedrijf.

De acht uitgangscaps zijn in revisie I geselecteerd als KEMET
C2220C476M3R2C7186 met grotere KPS-landpattern. Zie OUTPUT-CAPS-REVIEW.md
voor DC-bias, rimpelberekening en de nog open regelstabiliteitskwalificatie.

## Bronnen

- Vishay WSL, fabrikantdocument30100: https://www.vishay.com/docs/30100/wsl.pdf
- Littelfuse451/453: https://www.littelfuse.com/assetdocs/fuse-451-and-453-datasheet?assetguid=533cd5cc-956c-4243-867f-6ab5a62f6ba1
- Murata onderdeelreferentie: https://www.murata.com/en-global/api/pdfdownloadapi?cate=luCeramicCapacitorsSMD&partno=GRM32ER71H106KA12%23
- Panasonic onderdeelgegevens: https://industrial.panasonic.com/sa/products/pt/aluminum-cap-lead/models/EEUFR1H471
- Panasonic FR-A datasheet: https://industrial.panasonic.com/cdbs/www-data/pdf/RDF0000/ABA0000C1259.pdf

De volledige Murata DC-biasgegevens zijn nog niet verkregen. Deze beperking
is behouden in BOM en openpuntenlijst. Er zijn geen voorraad- of prijsclaims gedaan.
