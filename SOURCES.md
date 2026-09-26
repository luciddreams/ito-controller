# Fabrikantbronnen en wijzigingen K

Geraadpleegd24september2026; productvoorraad niet gecontroleerd.

- Mean Well GST160A: https://www.meanwell.com/Upload/PDF/GST160A/GST160A-SPEC.PDF —24V6,67A160W, R7B1/4plus2/3min, -V aanFG.
- TI TPS2663: https://www.ti.com/lit/ds/symlink/tps2663.pdf —TPS26630RGER, RGE0024H, actieve stroombegrenzing, IMON, PGOOD en externe reverse-block FET.
- Infineon IRLB4030: https://www.infineon.com/assets/row/public/documents/24/49/infineon-irlb4030-datasheet-en.pdf —100V, G-D-S,4,5mΩmax bij4,5V gate, ±16VGS.
- onsemi BSS138: https://www.onsemi.com/pdf/datasheet/bss138-d.pdf —G1/S2/D3, SOT23.
- TI SN74AHC1G08: https://www.ti.com/lit/ds/symlink/sn74ahc1g08.pdf —AND, DBV1A/2B/3GND/4Y/5VCC; PGOOD-pullup1kΩ.
- Kycon paneelconnector: https://www.kycon.com/Catalog_PDF/KPJX-PM.pdf —KPJX-PM-4S, matingKPPX.
- Phoenix1715721: https://www.phoenixcontact.com/nl-nl/producten/printplaatklem-mkds-15-2-508-1715721
- JST-XH: https://www.jst-mfg.com/product/pdf/eng/eXH.pdf
- Samtec socket: https://www.samtec.com/products/ssw-107-01-g-s
- Panasonic R25: https://industrial.panasonic.com/ww/products/pt/small-and-high-power-chip-resistors/models/ERJP06F1002V
- Murata bypass: https://pim.murata.com/en-global/pim/details/?partNum=GRM21BR71H104KA01%23
- TEC1-12706 referentiedatasheet: https://www.hebeiltd.com.cn/peltier.datasheet/TEC1-12706.pdf —gebruikersmodule kan van een andere fabrikant komen; test daarom bijlage/geleverde module.

Bestaande LT8613-, uitgangscap-, shunt- en BIAS-reviews staan als historische onderbouwing in history-rev-j. Die oude teksten bevatten inmiddels vervallen ingangstrappen en budgetten; de K-instructies zijn leidend.

- Seeed XIAO BLE officiële pinout: https://wiki.seeedstudio.com/XIAO_BLE/ en https://files.seeedstudio.com/wiki/XIAO-BLE/XIAO_nRF52840_front_pinout.png

## Firmware K.1

- Seeed nRF52 core 1.1.13: https://github.com/Seeed-Studio/Adafruit_nRF52_Arduino
- ADS1115 registers: https://www.ti.com/lit/ds/symlink/ads1115.pdf
- INA226 registers: https://www.ti.com/lit/ds/symlink/ina226.pdf
- MCP4728 sections 5.6.2 and 5.6.9: https://ww1.microchip.com/downloads/en/DeviceDoc/22187E.pdf

## Ambientuitbreiding L

Zie REMOTE-SENSORS.md voor NXP PCA9615, het officiële SparkFun EndPoint schema, Adafruit 2857 en de Sensirion SHT3x-DIS datasheet. Het adapterschema is gebruikt om de jumperinstellingen en RJ45-pinmapping te controleren.

## R7B-printingang J30 (2026-09-24)

Kycon KPJX-4S A13: https://www.kycon.com/Pub_Eng_Draw/KPJX-4S.pdf — 7,5A/contact bij48VDC, tekening onderaanzicht; gebruikt voor eigen footprint en vereenvoudigd STEP-model. Mean Well GST160A: https://www.meanwell.com/Upload/PDF/GST160A/GST160A-SPEC.PDF — R7B pin1/4 positief, pin2/3 retour.
