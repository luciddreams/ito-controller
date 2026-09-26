# Twee externe SHT31-meetpunten - revisie L

J28 en J29 zijn twee **extra** poorten op de hoofdprint voor een lokale differentiële I2C-adapter. Ze zijn geen rechtstreekse lange-kabelaansluitingen. Per poort is één adapter naast de controller én één adapter bij de externe sensor nodig. De PCA9615-adapters en SHT31-modules staan op de externe stuklijst, niet op de hoofd-PCB.

J26 bestond al in K als korte lokale I2C-aansluiting. Laat J26 leeg bij gebruik van beide externe SHT31's: alle drie de poorten delen dezelfde I2C-bus en twee identieke adressen tegelijk werken niet.

## Vastgelegde prototype-uitvoering

- Twee onafhankelijke kabeltrajecten, elk met twee **SparkFun QwiicBus EndPoint COM-16988** modules (vier modules totaal).
- Twee **Adafruit SHT31-D breakout, product 2857**. Deze module ondersteunt voeding én I2C-pull-ups op 5 V. Gebruik de beschreven Vin/GND/SDA/SCL-soldeeraansluitingen; een ander sensormoduleontwerp kan een andere voeding of levelshifter nodig hebben.
- Twee rechte, volledig bedrade CAT5e/CAT6-kabels met RJ45, aanvankelijk 1 m. Ontwerpdoel maximaal 3 m per tak bij maximaal 100 kHz, te verifiëren op de werkelijke installatie. Voor meer dan 3 m: nieuwe kabelkwalificatie; geen gegarandeerde onbeperkte lengte.
- De lange kabel voert differentiële SCL en SDA en 5 V/GND. **Geen Ethernet of PoE**: verbind deze RJ45's nooit met netwerkapparatuur, switches of PoE-injectoren.
- Ongebufferde bedrading controller-adapter maximaal circa 10 cm. Sensor-adapter circa 10-15 cm, met GND naast de signalen en gecontroleerde signaalflanken. Geen lange aftakkingen of Y-splitsingen op de differentiële zijde.

## Hoofdprint: J28/J29

Beide zijn JST XH 5-polig, 2,50 mm, B5B-XH-A(LF)(SN). Gebruik XHP-5-behuizingen en passende SXH-001T-P0.6 contacten. Nummering volgt de pin-1-markering van het PCB-footprint; controleer de kabel vóór insteken, niet op draadkleur vertrouwen.

| Pin | Signaal | Lokale EndPoint |
|---|---|---|
| 1 | +3V3, alleen lokale logica/referentie | VCC/VDDA, op de als 3.3V aangeduide lokale header |
| 2 | GND | GND |
| 3 | SDA, 3,3 V | SDA |
| 4 | SCL, 3,3 V | SCL |
| 5 | gezekerde +5 V | VCC1/VDDB, aparte voedingsheader |

F4 voedt J28:5; F5 voedt J29:5. Beide zijn 250 mA smeltzekeringen. Normaal verbruik per volledige tak maximaal 150 mA als prototypebudget; meet de werkelijke stroom. C51/C52 ontkoppelen de voedingspoorten. De 5 V komt uit M1, niet uit de 3,3 V regelaar van de XIAO. Beide takken samen reserveren maximaal 300 mA op de 5 V rail, naast de bestaande elektronica; totale 24 V budget blijft 4 A.

Pin 1 is geen 5 V ingang. Verbind de 3,3 V en 5 V rails nooit rechtstreeks. Bij programmeren met JP1 open: eerst USB, daarna 24 V; uitschakelen in omgekeerde volgorde, zoals in ASSEMBLY.md.

## Jumpers op alle vier EndPoint-modules

Deze instellingen zijn gecontroleerd tegen het officiële SparkFun QwiicBus EndPoint schema. Controleer de ontvangen revisie vóór solderen.

| Jumper | Twee lokale modules | Twee externe modules |
|---|---|---|
| `0-1` (VDDA-V DDB verbinding) | **OPEN**, anders wordt 5 V op de MCU-rail gezet | GESLOTEN: lokale sensorbus werkt hier op 5 V |
| `VCC1` | GESLOTEN, stuurt 5 V door de kabel | GESLOTEN, ontvangt 5 V |
| `GND` | GESLOTEN | GESLOTEN |
| `I2C` pull-ups | Beide traces OPEN: R1/R2 op de hoofdprint zijn de pull-ups | Beide traces GESLOTEN: lokale 4,7 kΩ pull-ups naar 5 V |
| `PWR` LED | Mag OPEN om verbruik/warmte te verminderen | OPEN om onnodige warmte bij de meetlocatie te beperken |

Behoud de 390/100/390-ohm differentiële netwerken van beide EndPoints. Voeg geen losse extra 100-ohm afsluitweerstanden toe. Dit is per tak een point-to-point verbinding met precies twee EndPoints.

De officiële EndPoint pinmapping van de RJ45 is: 1=DSCL-, 2=DSCL+, 3=GRN+, 4=VCC1, 5=GND, 6=GRN-, 7=DSDA-, 8=DSDA+. Het groene paar (3/6) blijft ongebruikt; plaats daarop geen externe voeding. Gebruik een rechte patchkabel, geen crossover. De mapping is ter kabelcontrole; sluit bij voorkeur de RJ45-stekkers rechtstreeks tussen de modules aan.

## Sensorzijde

Verbind de externe EndPoint VCC/VDDA (hier **5 V**, ook als de PCB-opdruk 3.3V aangeeft) met SHT31 Vin, en verbind GND/SDA/SCL. De gekozen Adafruit SHT31 heeft 10 kΩ pull-ups naar Vin; samen met de 4,7 kΩ op de externe adapter ontstaat circa 3,2 kΩ. De MCU-zijde blijft door de PCA9615 op 3,3 V.

- Sensor 1 op J28: ADR onverbonden (ingebouwde pulldown), adres `0x44`.
- Sensor 2 op J29: ADR verbinden met **eigen Vin**, adres `0x45`.
- RST en ALR mogen voor dit prototype onverbonden blijven.
- Sluit geen extra 3,3 V-only Qwiic-module op de externe 5 V sensorbus aan.

De PCA9615 heeft geen eigen I2C-adres; hij verandert de adressen niet. De poorten zijn elektrisch parallel aan de interne bus. Een defecte externe bus kan dus ook de interne metingen verstoren; de bestaande begrensde busdriver schakelt vermogen uit bij I2C-fouten. Dit ontwerp heeft geen galvanische isolatie en geen per-poort I2C-multiplexer.

## Plaatsing voor representatieve ambientmetingen

Plaats de sensorkop buiten de luchtstroom van het warme koelblok, de koude grenslaag bij het glas en directe zon/stralingswarmte. Een afstand van 1 m is een beginpunt, geen bewijs dat de lucht daar representatief is. Vergelijk de twee meetpunten met de installatie uit en tijdens koelen/verwarmen.

Houd de sensor met een kort dun kabeltje circa 10-15 cm van de EndPoint-print en monteer hem in een luchtige, geventileerde kap. Geen gezamenlijke dichte doos met de adapter. Vermijd direct thermisch contact met metalen chassis, kabelbundels met voedingsstromen, lijm of coating op de sensoringang. Bescherm tegen druppels en condens zonder de luchttoegang af te sluiten. Zet de interne SHT31-heater tijdens normale metingen uit; die zou temperatuur en RH beïnvloeden.

De firmware toont beide meetpunten afzonderlijk. Een gemiddelde van twee punten maakt een lokaal verstoorde meting niet automatisch correct. Bepaal pas na de praktijktest welk meetpunt als ambientreferentie voor de latere automatische regeling dient.

## Firmware L.1

Opdracht `ambient` toont per sensor adres, geldigheid, ouderdom, temperatuur in °C, RH in %, en dauwpunt in °C. Metingen zijn ongeveer één keer per seconde per sensor, single-shot high-repeatability zonder clock stretching. Beide CRC-bytes worden gecontroleerd. De firmware stuurt heater-off naar elke SHT31 bij initialisatie en na een communicatiefout.

Dauwpunt gebruikt de Magnus-benadering met a=17,62 en b=243,12 °C, voor ambient -20..60 °C en 0<RH<=100%. Het resultaat is dauwpunt boven vloeibaar water, niet het rijppunt boven ijs. Sensorresolutie is geen absolute nauwkeurigheid; de getoonde decimalen zijn voor diagnose.

Beide sensoren moeten standaard geldig en recent zijn voordat een vermogenspuls start (`REQUIRE_AMBIENT=true`). Een start vereist leeftijd <=1 s. Een actieve proef stopt bij verlopen/ongeldige meetdata (maximale leeftijd 3,5 s). Tijdens de maximaal 2 s heaterpuls wordt geen extra SHT31-busverkeer uitgevoerd, om de software-PWM niet te verstoren. Er is nog geen automatische dauwpuntregeling; dit voegt de meetbasis en foutcontrole toe.

## Kabel- en meetkwalificatie

1. Spanningsloos: controleer jumpers, kabelcontinuïteit, correcte 3,3/5 V scheiding en sensoradressen.
2. Eerst één tak op 1 m, vervolgens beide: meet spanning op de remote module, controleer CRC's en verse waarden.
3. Controleer alle vier PCA9615 VDDB-aansluitingen binnen 3,0..5,5 V; streef remote naar 4,5..5,25 V. Controleer lokale SDA/SCL hoog rond 3,3 V, nooit 5 V. Controleer lage niveaus en stijgtijden met de scoop.
4. Test opnieuw met de gewenste maximumkabel en de TEC-regelaars/fan/heater actief; registreer fouten, maximale dataleeftijd en ADC/NTC-continuïteit.
5. Trek één sensor/kabel los: fout of ontbrekende meetdata moeten zichtbaar zijn en vermogensvrijgave blokkeren/stoppen. Steek bij voorkeur spanningsloos om; hotplug is nog niet gekwalificeerd.
6. Leg minimaal 30 minuten data vast met de installatie uit en daarna thermisch belast. Controleer of de twee ambientpunten verschuiven door installatiegedrag; verplaats de meetkoppen als dat zo is.

Deze proeven zijn nog niet uitgevoerd; vul commissioning-results.csv pas met echte meetgegevens.

## Fabrikantbronnen

- NXP PCA9615: https://www.nxp.com/docs/en/data-sheet/PCA9615.pdf
- SparkFun EndPoint: https://www.sparkfun.com/sparkfun-qwiicbus-endpoint.html
- Officieel adapterschema: https://github.com/sparkfun/QwiicBus_EndPoint/blob/main/Hardware/SparkFun_QwiicBus_Endpoint.sch
- Adafruit SHT31-D pinout: https://learn.adafruit.com/adafruit-sht31-d-temperature-and-humidity-sensor-breakout/pinouts
- Sensirion SHT3x-DIS: https://sensirion.com/media/documents/213E6A3B/63A5A569/Datasheet_SHT3x_DIS.pdf
