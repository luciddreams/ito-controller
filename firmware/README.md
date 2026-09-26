# Prototypefirmware L.1

Bewaakte inbedrijfstelling voor PCB L, XIAO BLE nRF52840 en Mean Well 24 V. Dit is testfirmware met korte handmatige pulsen. Geen automatische condenscyclus, BLE-bediening of onbeheerd bedrijf. Er is nog geen echte print met deze firmware getest.

## Meegeleverd

- `ito_prototype/`: Arduino-broncode en `config.h`.
- `build/`: gecompileerde diagnoseversie, met vermogensvrijgave geblokkeerd.
- `calibrate.py`: tweepuntsberekening uit zelf gemeten waarden.
- `tests/`: hosttests van grenscontroles en de echte firmwaretoestanden met gesimuleerde hardware. De testconfiguratie is uitsluitend voor deze tests.
- `verification.txt`: werkelijke build- en testresultaten.

De USB-terminal geeft met `status` onder andere ruwe ADC-tellingen, gemeten ingangsstroom, heaterstroom, busspanning, vijf temperaturen, ventilatortoerental en foutstatus. Temperaturen en ingangsstroom zijn vóór kalibratie indicatief. Toestanden: 0=rust, 1=relaiscontrole, 2=heater, 3=TEC, 4=fout.

## Compileren en uploaden

Gebruik Arduino IDE met **Seeed nRF52 Boards 1.1.13**, bord **Seeed XIAO nRF52840**. Gebruik voor dit pakket niet de mbed-variant, Sense of Plus. TinyUSB 1.7.0 wordt met deze core geleverd; geen extra sensorbibliotheken nodig.

Board-index: https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json

Open `ito_prototype/ito_prototype.ino`. Of gebruik Arduino CLI:

```sh
arduino-cli core update-index --additional-urls https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
arduino-cli core install Seeeduino:nrf52@1.1.13 --additional-urls https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
arduino-cli compile --fqbn Seeeduino:nrf52:xiaonRF52840 --output-dir build ito_prototype
arduino-cli upload --fqbn Seeeduino:nrf52:xiaonRF52840 --port YOUR_PORT --input-dir build
```

Op Linux kan deze core `adafruit-nrfutil` op PATH vereisen: `python -m pip install adafruit-nrfutil`. De UF2 kan ook naar de bootloaderdrive worden gekopieerd na tweemaal kort op reset te drukken. Flash alleen met de 24 V uit en JP1 OPEN. Laat na inschakelen altijd de USB-voeding aanwezig zolang JP1 open is; schakel 24 V uit en ontlaad voordat USB wordt verwijderd. Zie ASSEMBLY.md.

## Eerste diagnose

Sluit nog geen glas of peltiers aan. Volg de elektrische inspectie in PROTOTYPE-START.md. Sluit de vijf juiste NTC's aan en controleer de NC-veiligheidsketen. Gebruik eerst belastingsweerstanden, een begrensde labvoeding, externe meters en een scoop.

De geleverde firmware zet heater-PWM, TEC_ARM en POWER_ARM laag vóór initialisatie. TCA-uitgangslatches gaan naar nul vóór de richtingwijziging; DAC-kanalen worden expliciet op nul en interne 2,048 V referentie gezet. De fan krijgt continu 100% vraag. Sensoruitval, hoge temperatuur, stroomoverschrijding, I2C-fout, veranderd configuratieregister, bedienknop loslaten of USB-verlies tijdens een proef zetten de directe vermogensvrijgaven uit en vergrendelen de fout. Geen automatische herstart van een proef.

I2C gebruikt een eigen begrensde open-drain driver op D4/D5, maximaal 100 kHz. Clock stretching heeft 1 ms timeout, een transactie 20 ms. Dit vermijdt de onbegrensde wachtlussen van Wire in deze core. De nRF-watchdog heeft een nominale timeout van 250 ms en wordt alleen na een complete lus gevoed. Softwaretiming en reactietijd moeten met een scoop worden gemeten; dit is geen gecertificeerde veiligheidsregeling.

## Kalibratie vóór vermogen

`MEASUREMENTS_VERIFIED=false`, nulwaarden voor de TEC-codegrenzen en ontbrekende heaterweerstand blokkeren standaard de proeven. Zet die vlag pas op true nadat de relevante meetwaarden en onderstaande controles zijn ingevuld. Voor de eerste weerstandstest mogen niet-gemonteerde TEC-kanalen op codegrens nul blijven. Beperk de labvoeding en start met een zeer kleine DAC-code; verhoog de codegrens alleen op basis van externe stroommeting. Een testgrens is geen voorspelling van de TEC-stroom.

1. Meet de 3,3 V rail en vul `VDD_V` in. Controleer het exacte R25- en beta-getal van alle vijf geïnstalleerde NTC's; de 3950 K in config.h is een placeholder. Bevestig hun aanwijzing bij omgevingstemperatuur en een tweede relevante temperatuur. Open en kortgesloten kabels moeten een fout geven. Gebruik bij afwijkende probes hun juiste omzetting in plaats van deze beta-benadering.
2. Kalibreer de ADC met twee bekende spanningen op de geïsoleerde meetingang, met de controller droog en vermogen uit. Belangrijk: koppel de te testen signaalbron eerst los; injecteer geen spanning tegen IMON of een aangesloten sensor. De interne ADC-schaal is nominaal 3,6 V/4096, niet 3,3 V/4095. Controleer beide ADC-kanalen. `status` geeft `raw=INPUT,HOT`.
3. Bereken de spanningsoverdracht met `python calibrate.py adc RAW1 VOLT1 RAW2 VOLT2`. Voer daarna een onafhankelijke derde controle uit. Vul slope en offset in `config.h` in. De onderschatting moet over het relevante bereik aantoonbaar onder 3% blijven, inclusief temperatuur en marge.
4. Kalibreer IMON met een externe elektronische belasting op +24_PROT, onder toezicht en zonder heater/TEC. Meet twee punten in het 2-4 A bereik. Bereken `python calibrate.py current IMONV1 AMP1 IMONV2 AMP2`. Controleer een derde punt. Firmware schakelt af bij 1,00 V óf 3,8 A gekalibreerd; de echte stroom moet onder 4 A blijven.
5. Verifieer de INA226 met externe meter en scoop. De firmware leest de shuntspanning rechtstreeks: 2,5 microvolt/bit over 10 milliohm. De softwaregrens is 3,4 A als marge onder 3,5 A. Beoordeel ook puls-piekstromen; alleen een gemiddelde meter is onvoldoende.
6. Meet de heaterweerstand tussen de complete busbars, of eerst de belastingsweerstand. Vul `HEATER_RESISTANCE_OHM` in. Met de gemeten spanning berekent de firmware een duty-bovengrens voor 10 W en controleert de verwachte aanstroom. Een dummybelasting gebruikt zijn eigen weerstand, niet die van het glas.
7. Kalibreer één TEC-kanaal per keer aan de weerstand, met de scope en stroommeter. Leg per kanaal een conservatieve `TEC_MAX_CODE` vast die inclusief rimpel, warme toestand en overshoot onder 2 A blijft. Vul nul in voor nog niet gekwalificeerde kanalen. Deze firmware activeert maximaal één TEC tegelijk.
8. Controleer fanpulsgetal en minimumtoerental. Voor een proef moet de fan al draaien. Controleer ook een losgetrokken tachkabel. Verifieer de ingestelde 2 s uitperiode met een scoop: stroom moet al zijn afgebouwd voordat een nieuwe modus wordt toegelaten.

## Proef uitvoeren

USB-terminal: 115200 baud, newline. J5 is de vasthoudknop: houd hem ingedrukt gedurende relaisinschakeling én de proef. Loslaten stopt en vergrendelt een actieve proef. Alle NC-veiligheidscontacten moeten operationeel zijn.

| Opdracht | Gedrag |
|---|---|
| `status` | Diagnosewaarden; geen vermogensactie |
| `heat 20 500` | Heater duty 20/255, 500 ms; alleen als binnen berekende 10 W grens |
| `tec 1 100 500` | Kanaal 1, DAC-code 100, 500 ms; alleen als code binnen gemeten kanaalgrens |
| `stop` | Directe GPIO-vrijgaven uit, daarna TCA en DAC nul |
| `reset` | Alleen met knop los; herinitialiseert apparatuur en wist fout uitsluitend bij gezonde meetwaarden en relais uit |

Opdrachten hierboven zijn voorbeelden, geen gekalibreerde stroominstellingen. Iedere proef wacht 200 ms op relais/busspanning voordat de uitgang actief wordt. Pulsen zijn 100-2000 ms, met minimaal 2 s uit tussen proeven. Heater-PWM is voorlopig 10 Hz. Start na een fout nooit door alleen de limiet te verhogen. Als een fout nog in de vorige meetcyclus staat, eerst `status` en pas daarna `reset`.

De firmware bewaakt vijf sensoren, maar weet niet of een correct aangesloten NTC mechanisch is losgeraakt van het glas. Controleer hechting en temperatuuruniformiteit fysiek. De onafhankelijke thermostaten blijven noodzakelijk.

## Praktijktests die nog openstaan

Vul `../commissioning-results.csv` met echte meetresultaten, waaronder kortgesloten/onderbroken I2C, watchdog, reset tijdens beide modi, sensorfouten, knop/USB-verlies, relaisfalen, fanstilstand, stroomgrenzen en thermische proeven. De brontests en een geslaagde build vervangen geen van deze metingen. Vier TEC's tegelijk en automatisch ontwasemen behoren pas tot een volgende firmwarestap na kwalificatie van één kanaal.

## Twee ambientmetingen (L.1)

Sluit de twee remote SHT31's volgens ../REMOTE-SENSORS.md aan. `ambient` toont T/RH/dauwpunt per sensor. Beide meetpunten zijn standaard vereist vóór proefpulsen; ontbrekende of te oude waarden blokkeren de start. De sensoren worden uit de externe 5 V tak gevoed: met alleen USB en 24 V uit zijn ze niet aanwezig. Dit is normaal en laat geen vermogen toe. Details over adressen, CRC, timing en de dauwpuntbenadering staan in REMOTE-SENSORS.md.
