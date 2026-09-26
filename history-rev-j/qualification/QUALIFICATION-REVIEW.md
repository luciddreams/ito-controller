# Verificatievervolg — PCB revisie I

Deze aanvulling verifieert de rekenmodellen en specificeert de fysieke proeven.
Er zijn GEEN metingen aan hardware verricht. Deze rekenreview is gebaseerd op revisie I. Revisie J heeft nieuwe ERC/DRC-rapporten
en een gewijzigde BIAS-voeding; BIAS-REVIEW.md beschrijft die aanvulling.

## Beslissingen uit deze review

1. Viermaal 12V/2A mag niet als gegarandeerde belasting over 20–28V worden gebruikt.
2. De circa 12V- en 2A-instellingen zijn geen harde beschermingsgrenzen.
3. C4 blijft 10nF: alleen een kleinere rampcondensator kiezen bewijst geen SOA.
4. Er is nog geen reden om op basis van alleen een LC-rekensom compensatie te wijzigen.
5. In revisie I lagen de BIAS-pinnen aan GND. Revisie J voedt ze vanaf VREG;
   zie BIAS-REVIEW.md. Het totale thermische gedrag blijft te kwalificeren.

## 1. Belastingsbudget met terugkoppeling van MOSFET-verlies

Eigen model: Vin×Iin − Rpair×Iin² = PTEC/η + Paux.
Gebruik dezelfde voorlopige aannames als de eerdere review: Rpair=0,252Ω,
η=0,85, Paux=15,125W (12W ventilator + 3,125W hulpvoeding). Overige bedrading-,
relais- en zekeringverliezen zijn niet inbegrepen. De lage positieve wortel is
gebruikt. De berekening verwerkt dat hogere stroom zelf meer MOSFETverlies geeft.

| Voeding | Ingangsstroom bij 96W TEC | TEC-budget bij 6A | Gelijke verdeling bij werkelijk 12V per TEC |
|---|---:|---:|---:|
| 20V | 7,03A | 81,43W | 1,70A per kanaal |
| 22V | 6,27A | 91,63W | 1,91A per kanaal |
| 24V | 5,67A | 101,83W | begrensd op nominale 2A |
| 28V | 4,78A | 122,23W | begrensd op nominale 2A |

In dit model vraagt 96W minimaal 22,86V om onder 6A te blijven. Er is bij die
waarde nog geen marge voor vergeten verliezen of slechter rendement. De tabel
is een grens voor werkelijke vermogens/stromen, geen lijst veilige DAC-codes.
Voor eerste proeven: één kanaal en gemeten belasting opvoeren; geen vierkanaals-
volvermogen onder 24V totdat een gevalideerde begrenzingsstrategie bestaat.

U3 meet heaterstroom, geen totale ingangsstroom. Zijn VBUS hangt achter F2 aan
ITO_PLUS: bruikbaar als belastingsbusindicatie onder gecontroleerde omstandigheden,
maar geen onafhankelijke VIN-meting vóór relais en zekeringen. LOAD_PRESENT_N is
slechts digitaal. Firmware kan dus niet claimen dat zij met deze print de totale
6A-ingangsstroom rechtstreeks meet en begrenst. Daarvoor is aanvullende meting
of een onderbouwd conservatief belastingbudget nodig. De voeding moet passend
begrenzen; een 10A-zekering verzorgt geen 6A-limiet.

## 2. Statische tolerantie en regelgedrag

Uit de gekozen 1,13MΩ/100kΩ-deler (beide 1%), de FB-referentiegrenzen en FB-bias
volgt een screening van ongeveer 11,55–12,33V. Dit omvat geen weerstand-TCR,
printlekkage, kabelverlies of overshoot. Een TEC met een absolute 12,0V-grens
is hiermee dus niet automatisch passend. De precieze TEC-grens ontbreekt nog.

Bij de gespecificeerde lage-common-mode-testconditie geeft 56mV en een 1% lage
25mΩ-shunt circa 2,26A. Dat is geen gegarandeerde bovengrens over het hele
uitgangsspanningsbereik. DAC-, buffer-, shunt- en temperatuurafwijkingen moeten
mee worden gekwalificeerd. ICTRL=0 is geen vervanger voor EN laag.

Eigen netwerkberekeningen:
- C109=10pF over R102=1,13MΩ: zero circa 14,1kHz en pool circa 173kHz.
- 10µH en 94µF: LC-frequentie circa 5,2kHz; 8µH en 50µF: circa 8,0kHz.
- 10kΩ/10kΩ met 100nF op DIV1: Thevenin-R=5kΩ, τ=0,5ms.

Dit zijn passieve netwerkfrequenties, GEEN berekende loopbandbreedte of fasemarge.
De fabrikant compenseert intern; zonder intern model of loopmeting kan een eigen
ideale bucksimulatie de stabiliteit niet certificeren. Voor de frequentiegevoeligheid
gebruikt calculate.py 450–550kHz als gekozen scenario rond 500kHz, niet als
gegarandeerde fabrikanttolerantie op deze RT-instelling.

Controleer in CC- én CV-bedrijf en bij hun overgang. De caps zitten vóór de
shunts; de Kelvin-pinnen zien de belastingstroom. Meet de differentiële rimpel
rechtstreeks aan ISP/ISN: ontwerpdoel onder 30mV p-p volgens fabrikantadvies.
Geen filter toevoegen voordat frequentie, ruisbron en fase-effect zijn vastgesteld.

## 3. Inschakelbelasting

De vier gekoppelde C4/laadstroomhoeken staan in calculated-results.json.
Bij 28V/600µF is de ideale ramp 8,4–25,7ms, met 0,655–2,00A laadstroom.
Snelle en langzame hoeken zijn afzonderlijke trajecten. De initiële dissipatie
van het lineaire pass-pad is in het model maximaal 56W en neemt daarna af.
De ideale pass-verliesenergie is 0,235J in ELKE hoek; alleen energie vergelijken
verbergt het verschil in SOA-belasting. Verdeel dit niet zonder bewijs gelijk
 over beide MOSFETs: meet Q2 en Q3 afzonderlijk.

Werkelijke gate-lading, hulpvoeding en ventilator zijn niet in dit rampmodel
opgenomen. Het relais uitschakelen verwijdert niet alle rechtstreeks op +24_PROT
 aangesloten belasting. Een verkeerd ingestelde stroombegrensde voeding kan het
ramptraject bovendien verlengen. Ook dát traject moet worden beoordeeld.
De beschikbare SOA-informatie sluit hete, herhaalde starts nog niet af.

analyze_capture.py berekent uit gelijktijdig gemeten VDS/ID/VGS de pieken en
integreert vermogen met werkelijke, eventueel ongelijke tijdstappen. Het geeft
bewust GEEN SOA-pass. Meetinstrumentbandbreedte, probevertraging, volledige
registratieduur en polariteit moeten eerst worden gecontroleerd.

## 4. Thermische marge

Het bestaande scenario bij 6A geeft 4,536W per ingangs-MOSFET. Met 10K/W per
apart koellichaam en 1,6K/W intern+interface wordt Tj circa 92,6°C bij 40°C lucht,
maar 112,6°C bij 60°C lucht. Met 15K/W en 60°C lucht wordt dit circa 135,3°C.
Dit maakt de lokale lucht en koellichaamkeuze bepalend. De aangenomen warme
weerstand is niet onafhankelijk van temperatuur; gebruik dit als screening.

In revisie I, met BIAS aan GND, betrekt de LT8613 zijn interne voeding uit VIN. Als rekenvoorbeeld
zou 5mA door een interne 3,4V-regelaar bij 24V ongeveer 0,103W dissiperen;
10mA circa 0,206W. Deze stromen zijn scenario's, geen vastgestelde werkstromen.
Meet de vier IC's afzonderlijk. Een goede ingang-MOSFETkoeling bewijst niets over
de warmteafvoer van de QFN's of de hete zijde van de Peltiers.

## Bronnen en reproduceerbaarheid

Projectwaarden: design.json en bijbehorende netten van PCB revisie I.
Fabrikantgegevens geraadpleegd 23 september 2026:
- LT8613, elektrische tabel en toepassingshoofdstukken: https://www.analog.com/media/en/technical-documentation/data-sheets/lt8613.pdf
- LTC4365, gate-ramp: https://www.analog.com/media/en/technical-documentation/data-sheets/ltc4365.pdf
- IRL540N, SOA en thermische gegevens: https://www.infineon.com/assets/row/public/documents/24/49/infineon-irl540n-datasheet-en.pdf

Run `python qualification/calculate.py` om de screening te reproduceren.
Run `python qualification/test_analysis.py` voor vier synthetische softwarechecks.
Hun succes is geen elektrisch meetresultaat. Zie BENCH-PROTOCOL.md voor het
meetplan en acceptance.json voor nog oningevulde fysieke resultaten.
