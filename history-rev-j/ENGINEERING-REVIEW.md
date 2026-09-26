# Engineeringreview — revisie I — 23 september 2026

## Ontwerpbudget, geen gemeten rating

24V ingang, voorlopig maximaal 6A. Heater en actieve TEC-regeling zijn onderling
vergrendeld. TEC-doel: vier kanalen van maximaal circa 12V/2A; de werkelijke
Peltierkeuze is nog nodig om koelvermogen en rendement te bepalen.

Bij 96W TEC-uitgang, aangenomen 85% omzetterrendement, 12W fanbudget,
3,125W hulpvoedingsingang en conservatief 9,1W ingangs-MOSFETverlies is het budget
137,2W of 5,72A bij 24V. Deze aannames zijn geen minimumrendementgarantie.
Bij onderspanning kan dezelfde belasting meer dan 6A vragen: firmware moet
vermogen beperken; voedingstolerantie en UV-drempel moeten samen worden getest.

## Ingangs-MOSFETs

IRL540NPBF is gekozen vanwege de 100V-rating, logic-level gate en beschikbare SOA-
informatie. LTC4365 levert volgens datasheet minimaal 7,4V gate-overdrive in dit
spanningsgebied. RDS(on) is maximaal 63mΩ bij 4,5V en 25°C. Als conservatieve
warme rekenwaarde wordt 2 × 63mΩ gebruikt, geen gegarandeerde datasheetwaarde.

Per MOSFET bij 6A: P = I²R = 4,54W. Met RθJC=1,1K/W, aangenomen interface 0,5K/W
en afzonderlijke koelplaat van 10K/W wordt de berekende temperatuurstijging
52,6°C. Bij 40°C lucht is dat circa 93°C junction. Deze vereenvoudigde berekening
vereist bevestiging van luchtstroming, montage en RDS(on) op temperatuur.
De drain-tabs voeren verschillende netten: elektrisch isoleren bij een gedeeld
koellichaam. Een gedeeld 10K/W-koellichaam is niet gelijkwaardig aan twee aparte.

Inschakelen: Cgate=10nF ±10%, gate-laadstroom 12–30µA. Bij 28V geeft het eenvoudige
rampmodel circa 8,4–25,7ms. Bij aangenomen maximaal 600µF beschermde belasting
is de capacitieve piekstroom maximaal 2A; ladingenergie 0,235J. Dit model negeert
nietlineaire gatecapaciteit en actieve lasten en kwalificeert geen SOA.
De langzame hoek wordt niet afgedekt door uitsluitend de 10ms-SOA-curve.
Meet VDS en ID samen bij koude en hete start voordat deze selectie vrijgegeven wordt.
LTC4365 is geen continue overstroomregelaar.

## Vier LT8613-kanalen

10µH ±20%, 500kHz. Bij 28V→12V en Lmin=8µH is de ideale rimpel circa 1,71A p-p;
bij 2A uitgang is de piek circa 2,86A. De gekozen Vishay-inductor heeft 9,33mΩ
maximale koude DCR en een typische verzadigingsstroom van 17A bij 20% inductieverlies.
Die typische waarde is geen gegarandeerde warme kortsluitgrens. De maximale
IC-piekstroomlimiet van 12A moet ook in foutbedrijf worden meegenomen.

Lokale 1µF/50V-VIN-bypass is verplaatst naast de IC. Twee bulk-ingangscaps blijven.
SW-verbinding naar bootstrap en spoel is korter; INTVCC-, RT- en SS-componenten zijn
verplaatst. Nieuwe voedingsvias zijn 1,0mm/0,5mm; kleine signaalvias 0,55mm/0,25mm.
Bestaande verbindingen kunnen andere maten behouden.

De inductorfootprint heeft padlengte 3,825mm en onderlinge binnenafstand 12,42mm;
dat komt overeen met de circa 12,41mm landpatternopening. Buitenwaarts is extra
soldeeroppervlak aanwezig. Padbreedte 12,32mm tegenover circa 12,45mm aanbevolen:
laat stencil/assembler dit bevestigen. Deze passing is geen thermische kwalificatie.

## Hardware-interlock

UCC27511 U2: IN+ = HEATER_PWM, IN− = TEC_ARM. De uitgang is alleen hoog wanneer
PWM hoog en TEC_ARM laag is. R7 is verlaagd naar 4k7 voor een sterkere lage
standaardwaarde. Test de interne inputbias tegenover deze pulldown bij reset.
De interlock beschermt tegen gelijktijdige actieve commando's, niet tegen een
kortgesloten MOSFET of vertraagd uitsterven van stroom in de TEC-uitgang.

## Glas en opwarmsnelheid

Met 8Ω/□, 90mm vrije afstand en 100mm breedte volgt ideaal 7,2Ω: 80W en 3,33A
bij 24V. Meet na aanbrengen van busbars; puntmetingen op ITO zijn niet equivalent.
Bij aangenomen glasdichtheid 2500kg/m³ en soortelijke warmte 840J/(kg K) is het
27,5g glas circa 23,1J/K. Ideaal: 3,46K/s, of 4,33s voor 15K stijging.
Peltiercontact, spiegelcoating, spreidplaat, verdamping en warmteverlies verlengen
dit. Het is nadrukkelijk geen voorspelde ontwasemingstijd of veilige boostduur.

## Bronnen

- Analog Devices LT8613: https://www.analog.com/media/en/technical-documentation/data-sheets/lt8613.pdf
- Analog Devices LTC4365: https://www.analog.com/media/en/technical-documentation/data-sheets/ltc4365.pdf
- Infineon IRL540N: https://www.infineon.com/assets/row/public/documents/24/49/infineon-irl540n-datasheet-en.pdf
- Vishay IHLP6767GZ-11: https://www.vishay.com/docs/34282/ihlp-6767gz-11.pdf
- TI UCC27511: https://www.ti.com/lit/ds/symlink/ucc27511.pdf
- TRACO TSR1, fabrikantdocument via Farnell: https://www.farnell.com/datasheets/2360311.pdf
- Murata GRM21BR71H105KA12L: https://search.murata.co.jp/Ceramy/image/img/A01X/G101/ENG/GRM21BR71H105KA12-01.pdf

Berekeningen hierboven zijn eigen screeningsberekeningen met expliciete aannames.

## Aanvulling I

Zie OUTPUT-CAPS-REVIEW.md voor de definitieve uitgangscapkeuze en de
capacitantiegevoeligheidsanalyse; THERMAL-BUDGET.md werkt het koelbudget uit.
Dit sluit de bestaande SOA-, stabiliteits- en prototypevrijgavepunten niet af.

## Aanvulling revisie J

De BIAS-voeding is naar de eigen uitgangsrail verplaatst. Zie BIAS-REVIEW.md
voor topologie, rekenvoorbeelden en de nog vereiste fysieke verificatie.
