# ITO controller — ontwerpgrenzen

De print is bedoeld als prototype voor een ITO-spiegel van 100 × 100 × 1,1 mm.
De vier Peltier-kanalen worden tijdens ontwasemen uitgeschakeld. De heater heeft
bij 24 V en 7,2 Ω nominaal 80 W elektrisch vermogen. Dit is geen toegestane
thermische instelling zonder kwalificatie van glas, busbars en coatings.

## Aansluiten

- XIAO BLE nRF52840 op J1/J2, twee vrouwelijke 1×7 headers. USB richting onderrand.
- JP1 open tijdens USB-voeding; sluit de hulpvoeding niet ongecontroleerd parallel
  aan USB. Controleer de 5V-voedingsroute van de daadwerkelijke XIAO-variant.
- Vermogensvoeding 24 V DC. Vier onafhankelijke TEC-uitgangen, geen ompoolbruggen.
- Fan-aansluiting is voor de bestaande 24V-opzet. Een ARCTIC 12V-fan heeft een
  afzonderlijke 24→12V-omzetter en passende signaalinterface nodig. Niet op 24 V aansluiten.
- Externe hardwarethermostaten/noodstop in de daarvoor getekende relais-spoelkring.
- De connectorvolgorde staat in connector-pinout.csv; deze is geen garantie dat
  een aangekochte sensor- of ventilatorkabel dezelfde volgorde gebruikt.
- 0,5mm dikke koperen busbars eerst vlakmaken, draden vooraf solderen en aan
  het frame ontlasten. De ITO-laag mag niet door spiegelmetaal worden kortgesloten.

## Verificatie vóór fabricage/assemblage

Native ERC, DRC en schema/PCB-netpariteit dekken verbindingen en geometrische regels,
geen schakelstabiliteit, EMC, maximaal toelaatbare stroom of temperatuur.

1. M1, Q2/Q3, L1–L4 en lokale VIN-bypass hebben nu gekozen bestelnummers.
   Zekeringen, shunts, ingangscaps en C5 zijn in H geselecteerd. Uitgangscaps zijn in I geselecteerd;
   connectors blijven te kiezen; de volledige componentkwalificatie is nog niet afgerond.
2. Beoordeel de LT8613-layout tegen het fabrikantvoorbeeld: lokale VIN/GND-lus,
   SW-oppervlak, bootstrap en feedback. Kelvin-topologie is nu geometrisch gecontroleerd;
   controleer nog meetruis, soldeerlandpattern en absolute meetnauwkeurigheid.
3. Bepaal de fabrieksstackup. De intentie is 4 lagen, 1,6mm, buiten 70µm en
   binnen 35µm koper. Baanbreedte en via-aantal moeten op de werkelijke stroom,
   koperopbouw en temperatuurstijging worden beoordeeld.
4. QFN exposed pads bevatten open thermische vias. Bepaal de soldeerproceskeuze
   (vullen/afdekken of paste-aanpassing) met de assembler; voorkom soldeerwegvloeiing.
5. Controleer XIAO-mechanica, antennezone, connectororiëntatie en koelruimte voor
   de TO-220-onderdelen tegen echte onderdelen.
6. Test eerst één TEC-regelaar en de heater aan een begrensde voeding. Controleer
   stroomregeling, uitschakeling bij sensorfouten, relaisgedrag en spanningssprongen.
7. Bepaal glas-temperatuurgrenzen en boostduur met temperatuurmetingen op meerdere
   plaatsen. Houd koelventilatorlucht achter de spiegel en de elektronica droog.

Geen bestelling van productieborden op basis van uitsluitend een groen ERC/DRC-resultaat.

## Aanvulling revisie H

Voorlopig rekenbudget: 24V, maximaal 6A ingang; dit is geen vrijgegeven rating.
Gebruik Q2/Q3 alleen met berekende koeling; zie ENGINEERING-REVIEW.md.
Hardware: TEC_ARM=1 blokkeert U2 ongeacht HEATER_PWM. Dit voorkomt gelijktijdige
actieve aansturing, maar verwijdert geen restwarmte of opgeslagen uitgangsenergie.
Firmware moet bij omschakelen eerst PWM nul maken, DAC-stroom nul maken en pas
na gemeten stroomafbouw de andere modus vrijgeven. Verifieer reset en brownout.

## Onderdelen en zekeringen — revisie H

Gebruik de nieuwe BOM en fabrikantfootprints samen. F11–F14 zitten aan de
INGANG van de regelaars en zijn 2,5A; dit verhoogt de ingestelde TEC-stroom niet.
Een stroombegrensde 6A-voeding kan F1=10A bij een fout heel laten. Vertrouw niet
op F1 voor een exacte stroomlimiet of snelle MOSFET-bescherming.
Voor C5 is minimaal 22mm hoogte vrij te houden, plus montage-/ventilatieruimte:
20mm nominale lengte met +2mm tolerantie volgens fabrikanttekening.

## Uitgangscaps — revisie I

Monteer de gekozen KEMET KPS-onderdelen volgens het fabrikant-reflowprofiel.
Reserveer minimaal 5,5mm componenthoogte plus mechanische speling. De oude
1210-uitgangsfootprints zijn vervallen. Gebruik schema, PCB en BOM van dezelfde revisie.
Zie OUTPUT-CAPS-REVIEW.md en THERMAL-BUDGET.md voor de nieuwe berekeningen.

## BIAS-wijziging — revisie J

U11–U14 pin22 is nu verbonden met de eigen uitgang vóór de shunt. Monteer of
bedraad die pin niet volgens revisie I aan massa. Zie BIAS-REVIEW.md voor de
werking en de extra test rond3,1V uitgangsspanning.
