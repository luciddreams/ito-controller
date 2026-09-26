# Assemblage en externe bedrading

## Onderdelen en mechanica

`BOM-prototype.csv` bevat één regel per componentpositie. MPN's leggen de elektrische uitvoering vast; actuele voorraad en minimale bestelverpakking zijn niet gecontroleerd. Bij generieke passieven mag de assembler dezelfde waarde, footprint en minimaal opgegeven spanning/vermogen/tolerantie leveren. U1, Q2/Q3, shunts, spoelen, KPS-uitgangscaps en precisiedelers niet zonder review vervangen.

- XIAO BLE nRF52840: twee Samtec SSW-107-01-G-S sockets, rijafstand15,24mm, pinsteek2,54mm. USB naar de onderrand van de PCB. Monteer de headers met de XIAO als spanningsloze uitlijnmal; neem de module eruit vóór reflow of vermogensproeven.
- JP1-connector wel monteren; shunt standaard NIET plaatsen. USB-voeding en +5_AUX mogen niet ongecontroleerd parallel. Tijdens USB-gevoede vermogensproeven: eerst24V uitschakelen en de belastingsbus laten ontladen, daarna pasUSB losnemen; zo blijft de ADC niet doorIMON gevoed terwijl de MCU-rail uit is. Voor standalone gebruik: USB los, spanningsloos JP1 sluiten, dan24V inschakelen.
- C5: reserveer22mm hoogte plus speling. KPS-uitgangscaps: minimaal5,5mm hoogte plus speling. Geen frameklem tegen deze onderdelen.
- Q1/Q2: TO-220 rechtop, G-D-S volgens footprint. De metalen tabs zijn elektrisch drain; niet tegen een geleidend frame drukken. Q2 heeft door zijn lage weerstand voor4A nominaal geen apart koelblok nodig; meet de temperatuur. Voor Q1 bepaalt de proef met PWM en werkelijke heater de koelbehoefte.
- Vier M3-montagegaten: gebruik isolerende afstandhouders en houd behuizing/koellichaam van de koperzijde af. Geen warme Peltier-uitblaaslucht over de PCB.
- De spiegelende metaallaag moet elektrisch van de ITO-heater geïsoleerd blijven. Ontlast de draden mechanisch bij de0,5mm-koperen busbars; niet aan het glas laten trekken.

## Mean Well GST160A24-R7B rechtstreeks op J30

Monteer **J30 = Kycon KPJX-4S**, de niet-afgeschermde vierpolige printbus. De R7B-stekker van de GST160A24-R7B gaat rechtstreeks in J30 aan de onderrand. Geen verloop en geen externe paneelbus nodig. Bestel precies KPJX-4S; de -S/-PM-uitvoeringen hebben andere mechanische details.

| Fabrikantpin J30 | Functie |
|---|---|
| 1 en 4 | +24_IN, beide aangesloten |
| 2 en 3 | GND / retour, beide aangesloten |

De nummers volgen de fabrikantmarkeringen. De footprint is het gespiegeld onderaanzicht uit Kycon-tekening A13. Controleer de fysieke component en polariteit voordat je inschakelt. Het connectorfront steekt 4 mm voorbij de PCB; maak ruimte voor de stekker en vergrendeling in de behuizing. De connector is opgegeven voor 7,5 A per contact bij 48 V DC. **De print blijft een <=4 A prototype** met de bestaande beveiliging; de connector is geen vrijgave voor meer belasting.

J3 blijft een alternatieve tweepolige voedingsingang voor een labvoeding. J3 en J30 zijn direct parallel aangesloten en dus beide onder spanning als een van beide wordt gevoed. Sluit nooit twee voedingen tegelijk aan. J3 pin1 = +24_IN, pin2 = GND. De klemopening van J3 wijst nu naar de vrije ruimte boven de klem.

Het eerder genoemde DC PLUG-P1J-R7B verloop heeft de verkeerde richting voor de R7B-uitgang van deze voeding. Voor dit ontwerp is ook het omgekeerde R7BF-P1J niet nodig.

De Mean Well-datasheet geeft aan dat -V verbonden is met AC FG. Dat is relevant bij USB-aansluiting en geaarde meetapparatuur.

## Overige aansluitingen

`connector-pinout.csv` is leidend voor alle fysieke pinnummers. Phoenix1715721-terminals zijn5,08mm, JST-XH-signaalconnectoren zijn2,50mm en de XIAO-headers2,54mm; dit is niet onderling uitwisselbaar.

- J4 naar busbars, bij voorkeur1,0mm² soepele draad, kort en getwist.
- J11–J14 naar de vier TEC's, minimaal0,5mm² per ader voor deze2A-proeven. De TEC's komen op afzonderlijke kanalen, niet parallel op één uitgang. Bepaal de koud/warm-polariteit bij lage stroom.
- J5: drukknop NO naarGND. J9: aparte eFuse-resetknop NO, SHDN naarGND. J9 is geen noodstop.
- J6: NC-noodstop; J7: NC-glasthermostaat; J8: NC-heatsinkthermostaat. Gebruik contacten geschikt voor24VDC en de relaisspoelstroom, bij voorkeur handreset voor thermostaten. Bij automatisch herstellende thermostaten moet firmware een fout vergrendelen; dit vervangt de bewaakte eerste proeven niet.
- J21–J24: vier10kΩ-NTC's voor het glas; J25:10kΩ-NTC op de warme warmtewisselaar. Kies één gespecificeerd beta-type en pas de firmwarecurve daarop aan;10kΩ alleen bepaalt de temperatuurcurve niet. Meet alle sensoren vóór lijmen.
- J26:3,3V-I²C-luchtvochtigheidssensor. Gebruik een3,3V-module zonder5V-pull-ups en controleer zijn kabelpinout.
- J27:24V-fan, maximaal0,5A voor het prototype; tach moet open-collector/open-drain zijn. De PWM-uitgang is een transistor die naarGND trekt, geen24V-logicasignaal. Controleer de pinvolgorde van de fan zelf en bedraad een adapter; een standaard PC-fanstekker is geen bewijs van24V-geschiktheid.

Voor de eerste elektronische proeven kunnen NTC's worden vervangen door bekende10kΩ-weerstanden. Doe dat niet tijdens glas- of TEC-verwarmingsproeven: dan moeten de temperatuursensoren echt gemonteerd zijn.

XIAO-positie, bovenaanzicht: J1 rechts (x=144,29 mm), J2 links (x=129,05 mm); beide pin 1 op y=104 mm. Afstand tussen rijen 15,24 mm. Modulecomponenten omhoog en USB naar de onderrand.

## Extra ambientpoorten L

Monteer J28/J29 en de bijbehorende F4/F5 (250 mA) en C51/C52 (1 microfarad). Volg REMOTE-SENSORS.md voor de vier externe EndPoint-modules en twee SHT31's. J28/J29 zijn lokale 3,3 V I2C-poorten met aparte 5 V voeding, geen directe lange-kabelpoorten.
