# BIAS-voeding van de TEC-regelaars — revisie J

## Uitgevoerde wijziging

U11–U14 pin22 (BIAS) zijn van GND verplaatst naar de eigen T1–T4_VREG-rail.
Elke verbinding loopt afzonderlijk naar pad1 van C107/C207/C307/C407,
rechtstreeks op de uitgangscondensator vóór de 25mΩ-shunt. Daardoor loopt het
BIAS-verbruik niet door de shunt en telt het niet mee als gemeten TEC-stroom.
De vier oude verbindingen tussen pin22 en massa zijn verwijderd.
Er zijn geen nieuwe vermogensonderdelen toegevoegd; de verbindingen zijn 0,15mm
breed voor de kleine voedingsstroom van deze pin. De geometrische audit controleert
afzonderlijk dat zij pas op de condensatorpad bij het vermogensnet aansluiten.

De vier post-inductor-rails zijn met POWER_PRESENT-symbolen in het schema als
gevoede rails aangeduid. Deze ERC-symbolen leggen vast dat de rail via spoel en
schakelregelaar wordt gevoed; ze zijn geen fysieke onderdelen of regeluitsluitingen.
Een na het herleggen geïsoleerd bovenzijdig massa-eiland bij kanaal2 is met een
0,70mm-via, boring0,35mm, verbonden met het interne en onderste massavlak.

## Onderbouwing en grenzen

Analog Devices adviseert BIAS bij geschikte uitgangsspanning uit VOUT te voeden.
Boven ongeveer3,1V kan de interne voeding uit BIAS komen; bij lage spanning
wordt VIN gebruikt. De absolute BIAS-limiet is25V. Onze nominale VREG van ongeveer
12V ligt daaronder, inclusief de eerder doorgerekende statische toleranties.
Deze vergelijking dekt geen defecte omzetter of ongecontroleerde transiënt.
De aansluiting ligt op de eigen ontkoppelde uitgang; het is geen aparte externe
BIAS-voedingsbron. INTVCC houdt zijn bestaande lokale1µF-ontkoppeling.

Eigen rekenvoorbeeld bij24V in en12V uit: bij dezelfde5mA interne stroom neemt
het lineaire regelverlies af van(24−3,4)×5mA=103mW naar(12−3,4)×5mA=43mW.
Dat is circa60mW per IC of240mW over vier kanalen. Bij10mA is het verschil
120mW per IC. Dit zijn stroomscenario's, geen gemeten besparing of rendementsgarantie.
Bij lage TEC-spanning onder de BIAS-omschakeldrempel bestaat deze winst niet.
Het nieuwe uitgangsverbruik moet door de converter worden geleverd; de werkelijke
verandering van het totale ingangsvermogen moet worden gemeten.

## Controles en resterende proeven

De nieuwe netaansluitingen zijn tussen schema en PCB vergeleken. Controleer de
bijgevoegde actuele ERC/DRC-, Kelvin- en BIAS-topologierapporten.
De elf Kelvin-meettakken blijven gescheiden tot hun respectieve shuntpad.
Geometrische topologie bewijst geen afwezigheid van capacitieve/inductieve koppeling.

Aanvullende prototypeproef: doorloop uitgangsspanningen van0–4V langzaam in CC-bedrijf,
vooral rond3,0–3,3V. Meet BIAS, INTVCC, VIN, uitgang en belastingstroom tijdens
inschakelen, dalende stroomopdracht, omschakeling en uitschakelen. Zoek naar
oscillatie, afwijkende stroom of onverwachte herstart bij interne bronwisseling.
Herhaal daarna de staprespons en thermische metingen uit qualification/BENCH-PROTOCOL.md.
De eerdere SOA-, voeding-/TEC-selectie- en thermische vrijgavepunten blijven open.

Bron: Analog Devices LT8613 Rev.A, pinbeschrijving BIAS en sectie INTVCC Regulator,
geraadpleegd23september2026:
https://www.analog.com/media/en/technical-documentation/data-sheets/lt8613.pdf
