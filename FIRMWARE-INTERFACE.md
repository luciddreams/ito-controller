# Firmwarecontract voor prototype L

Dit pakket bevat de PCB, testinstructies en gecompileerde prototypefirmware L.1 in `firmware/`. De firmware heeft hosttests doorstaan, maar is nog niet op hardware getest. De print kan onbestukt of met ongeprogrammeerde XIAO worden vervaardigd; vermogensuitgangen horen dan uit te blijven.

| XIAO-label | Net / functie |
|---|---|
| D0 | HOT_ADC, hete-kant-NTC |
| D1 | INPUT_I_ADC, totale stroom via eFuse IMON; GEEN3,3V-monitor meer |
| D2 | LOAD_PRESENT_N, laag bij spanning achter relais |
| D3 | HEATER_PWM |
| D4 / D5 | SDA / SCL,3,3V |
| D6 | TEC_ARM; hoog blokkeert heaterdriver |
| D7 | FAN_PWM transistor |
| D8 | FAN_TACH |
| D9 | POWER_ARM; AND met PGOOD naar relais |
| D10 | BUTTON_N |

I²C: TCA9534=0x20; INA226=0x40; ADS1115=0x48; MCP4728=0x60 bij de getekende adressen. Controleer de daadwerkelijke apparaten vóór vrijgave. TCA9534 P0–P3 zijn kanaal-enables; P4 leest PROT_FAULT_N.

## Opstart en fouten

Zet POWER_ARM, HEATER_PWM en TEC_ARM direct laag. Initialiseer I²C en schrijf eerst TCA-uitgangsdata0, daarna P0–P3 als uitgang en P4 als ingang. Zet alle DAC-kanalen expliciet0 voordat EN of relais aan kan. Vertrouw niet op bewaarde DAC-EEPROMwaarden. Controleer5sensoren, eFuse-foutstatus en relaisuitstatus. Gebruik watchdog en een vergrendelde foutstatus; I²C-timeout, ongeldige sensor of brownout moet de vermogensvrijgaven laten afvallen.

R19 houdt POWER_ARM tijdens reset laag, R33 houdt HEATER_PWM laag, R7 houdt TEC_ARM laag; bestaande kanaal-pulldowns houden LT8613 uit. U15 kwalificeert relaisvrijgave met eFuse PGOOD. Dit is geen gecertificeerd veiligheidssysteem.

Bij koelen: eerst heaterPWM0, stroomafbouw bevestigen, TEC_ARM hoog, dan de gevraagde kanalen vrijgeven. Bij ontwasemen: DAC0, kanaal-EN laag, stroomafbouw bevestigen met geschikte externe meting tijdens kwalificatie, dan TEC_ARM laag en heaterPWM toestaan. De print heeft geen afzonderlijke digitale stroommeting per TEC; vertrouw dus niet op niet-bestaande feedback. Kies een conservatieve vaste omschakelpauze pas na scopeverificatie op de prototypekanalen.

## Stroommeting en grenzen

IMON geeft typisch27,9µA per ampère. R5=10kΩ geeft typisch0,279V/A; R4=0Ω en R6/C3 vormen verbinding en ADC-filter. Gebruik voor de ADC de juiste nRF52840-gain/referentie en een bekende resolutie. Gebruik niet zonder controle `raw/4095*3.3`.

In het2–6A-bereik is de gebruikte datasheetgain26,22–29,58µA/A. Voor ongekalibreerd bedrijf: software-afschakeling bij maximaal1,00V op INPUT_I_ADC, mits de totale ADC-onderschatting aantoonbaar≤3% is. Dit correspondeert in die foutaanname met minder dan4A werkelijk. Kalibreer daarna twee punten en handhaaf4,0A als echte bovengrens met marge. Onder2A is dit geen precisie-ampèremeter. Middel voor vermogensregeling, maar behoud een snelle foutdetectie; de eFuse is de onafhankelijke hardwarebegrenzing.

INA226 meet alleen heaterstroom:10mΩ-shunt, CAL=0x0800 voor0,25mA/bit. Stem middeling/conversietijd af opPWM; piekstroom mag niet worden verborgen door een laag gemiddelde. Houd werkelijke heater-aanstroom onder3,5A voor het prototype; PWM verlaagt het gemiddelde, niet de aanstroom.

TEC-stroom: interne DAC-referentie2,048V, gain1 en de10k/10k-deler leveren maximaal circa1,024V opICTRL. Datasheet-/shunttoleranties betekenen dat nominale2A geen harde2A-grens is. Begin met lage codes en leg per kanaal een gemeten codegrens vast op maximaal2,0A, inclusief rimpel/overshoot en warme proef. Totale ingangsstroom heeft altijd voorrang op koelvraag.

Voor eerste glasproeven:10W beginvermogen, voorlopige40°C-softwarestop, maximaal2s-pulsen met volledige temperatuurcontrole tussen de pulsen. Meer vermogen/duur pas na de testreeks. Voor de warme kant aanvankelijk55°C-softwarestop en onafhankelijke circa60°C-NC-thermostaat; stel lager in als gebruikte onderdelen dat vragen. Ongeldige NTC-waarden, open kabel en kortgesloten sensor moeten als fout gelden, niet als zeer lage temperatuur.

De prototypefirmware gebruikt een begrensde software-I²C-driver op maximaal 100 kHz; R1/R2 zijn 2,2 kΩ. Gebruik de D-labelconstanten van de gekozen XIAO-boarddefinitie, geen ongecontroleerde numerieke Arduino-pinnummers: deze kunnen tussen cores verschillen.

SHT31 ambient: 0x44 en 0x45, geen adresconflict met interne onderdelen. Firmware L.1 controleert beide CRC's, dataleeftijd en geldigheid; zie REMOTE-SENSORS.md. J26 blijft leeg bij deze twee remote sensoren.
