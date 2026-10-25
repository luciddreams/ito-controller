# Eerst de XIAO zelf testen

1. Haal de XIAO van de spanningsloze controllerprint en sluit hem alleen via USB aan.
2. Installeer in Arduino IDE het boardpakket **esp32 by Espressif Systems 3.3.2**.
3. Open `ito_prototype_esp32s3/ito_prototype_esp32s3.ino` met alle bijbehorende headers in dezelfde map.
4. Kies **XIAO_ESP32S3**, **USB Mode: USB-OTG (TinyUSB)** en **USB CDC On Boot: Enabled**. PSRAM mag uit blijven.
5. Upload. Open de seriële monitor op **115200 baud**, met **Newline**.
6. Stuur `status` en vervolgens `pins`. Fouten over ontbrekende I2C-apparaten zijn normaal zolang de module los van de print is.

De meegeleverde instellingen blokkeren vermogensuitgangen totdat de metingen
gevalideerd zijn. Alleen U11 staat als geplaatst ingesteld. Deze versie is
gecompileerd en softwarematig getest, nog niet op jouw print getest.

Voor montage: alle voedingen los, **JP1 verwijderen** voor USB-voeding en de
oriëntatie controleren. Lees eerst de aansluitvoorwaarden in README.md.

Let op: **D6/GPIO43 (TEC_ARM) kan tijdens opstarten UART-signalen geven voordat
de firmware draait**. Controleer daarom eerst de werking van relais K1 en de
kanaalblokkering, zonder TEC of heater aangesloten. Een eventuele TP1-overbrugging
moet weg. De eerdere 20,9V op F11 zonder microcontroller vraagt om controle van
dit stroompad. Een software-instelling lost dat niet op.

Seeed verlangt een diode bij externe voeding via de 5V-pin. JP1 is geen diode.
Gebruik voor deze eerste tests USB met JP1 open. Voed eerst de XIAO via USB en
schakel daarna pas de printvoeding in; schakel bij stoppen eerst de printvoeding
uit en laat de print ontladen voordat USB losgaat.
