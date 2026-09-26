# Manufacturer sources and design references

Original design references were consulted on 24 September 2026. Datasheet downloads were collected on 26 September 2026. Stock/prices are not verified. The full document inventory, retrieval status and hashes are in [datasheets/README.md](datasheets/README.md) and `datasheets/manifest.json`.

| Reference | Design relevance |
|---|---|
| [Mean Well GST160A](https://www.meanwell.com/Upload/PDF/GST160A/GST160A-SPEC.PDF) | 24 V / 6.67 A / 160 W; R7B pins 1/4 positive and 2/3 return; -V connected to FG |
| [TI TPS2663](https://www.ti.com/lit/ds/symlink/tps2663.pdf) | TPS26630RGER, RGE0024H package, active current limiting, IMON, PGOOD and external reverse-block FET |
| [Infineon IRLB4030](https://www.infineon.com/assets/row/public/documents/24/49/infineon-irlb4030-datasheet-en.pdf) | 100 V, G-D-S, maximum 4.5 mΩ at 4.5 V gate, ±16 VGS |
| [onsemi BSS138](https://www.onsemi.com/pdf/datasheet/bss138-d.pdf) | G1/S2/D3, SOT23 |
| [TI SN74AHC1G08](https://www.ti.com/lit/ds/symlink/sn74ahc1g08.pdf) | AND gate, DBV pins 1A/2B/3GND/4Y/5VCC; 1 kΩ PGOOD pull-up |
| [Kycon KPJX-4S A13](https://www.kycon.com/Pub_Eng_Draw/KPJX-4S.pdf) | Current J30 PCB socket, 7.5 A/contact at 48 V DC; bottom-view drawing used for local footprint and simplified STEP |
| [Kycon KPJX-PM](https://www.kycon.com/Catalog_PDF/KPJX-PM.pdf) | Earlier panel-socket option; not the current J30 part |
| [Phoenix 1715721](https://www.phoenixcontact.com/nl-nl/producten/printplaatklem-mkds-15-2-508-1715721) | Two-pin PCB terminal |
| [JST-XH](https://www.jst-mfg.com/product/pdf/eng/eXH.pdf) | Signal connectors and cable contacts |
| [Samtec SSW-107-01-G-S](https://www.samtec.com/products/ssw-107-01-g-s) | XIAO sockets |
| [Panasonic ERJP06F1002V](https://industrial.panasonic.com/ww/products/pt/small-and-high-power-chip-resistors/models/ERJP06F1002V) | R25 |
| [Murata GRM21BR71H104KA01](https://pim.murata.com/en-global/pim/details/?partNum=GRM21BR71H104KA01%23) | Bypass capacitor |
| [TEC1-12706 reference](https://www.hebeiltd.com.cn/peltier.datasheet/TEC1-12706.pdf) | Reference only; purchased modules may use another manufacturer and require characterization |
| [Seeed XIAO BLE](https://wiki.seeedstudio.com/XIAO_BLE/) | Official pinout; [pinout image](https://files.seeedstudio.com/wiki/XIAO-BLE/XIAO_nRF52840_front_pinout.png) |

Historical LT8613, output-capacitor, shunt and BIAS reviews remain in `history-rev-j/`. They contain superseded input stages/budgets; current revision-L instructions take precedence.

## Firmware references

- [Seeed nRF52 core](https://github.com/Seeed-Studio/Adafruit_nRF52_Arduino), version 1.1.13 used by this project.
- [ADS1115 registers](https://www.ti.com/lit/ds/symlink/ads1115.pdf).
- [INA226 registers](https://www.ti.com/lit/ds/symlink/ina226.pdf).
- [MCP4728](https://ww1.microchip.com/downloads/en/DeviceDoc/22187E.pdf), including sections 5.6.2 and 5.6.9.

## Revision-L ambient extension

[REMOTE-SENSORS.md](REMOTE-SENSORS.md) records NXP PCA9615, the official SparkFun EndPoint schematic, Adafruit 2857 and Sensirion SHT3x-DIS sources. The adapter schematic was used to verify jumpers and RJ45 pin mapping.
