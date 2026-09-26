> **Bijgewerkt 25 september 2026:** Schema opnieuw getekend met traditionele symbolen, echte verbindingsdraden en 13 functionele bladen plus overzicht. Zie `SCHEMA-LEZEN.md` en de schema-PDF. Alle 202 componenten / 638 pinnen elektrisch gelijk; ERC 0. PCB en fabricagebestanden ongewijzigd.

> **Bijgewerkt 24 september 2026:** J30 (Kycon KPJX-4S) voor de R7B-voedingsstekker toegevoegd aan de onderrand. J3/J11-J14 gedraaid voor vrije stekker-/draadinvoer. C5 en weerstandencluster gecorrigeerd. Zie `R7B-UPDATE.md` en `PLACEMENT-UPDATE.md`.

# ITO-controller revisie L - prototypepakket

Dit pakket is gereed voor fabricage van een eerste PCB-prototype. Het is geen vrijgave voor serieproductie of onbeheerd gebruik. Er is nog geen gebouwde print getest. De meegeleverde prototypefirmware L.1 is gecompileerd en heeft hosttests doorstaan; werking op de echte print is nog niet bewezen.

## Bestellen en opbouwen

1. Gebruik `manufacturing/ito-l-fabrication.zip` voor de kale PCB en neem alle eisen uit `FABRICATION.md` over. Vier lagen, 160 x 110 mm, 1,6 mm dik, 70/35/35/70 micrometer koper. De 16 aangegeven QFN-vias moeten gevuld en afgedekt worden. Laat de leverancier zijn normale DFM-controle uitvoeren.
2. Gebruik `BOM-prototype.csv`, de SMT-positioneringslijst en montagetekeningen in `manufacturing/` voor assemblage. Voorraad en prijzen zijn niet gecontroleerd. Assembleer eerst één print.
3. Volg `ASSEMBLY.md` voor XIAO, voeding en externe bedrading. Gebruik alleen revisie L; `history-rev-j/` is archief.
4. Volg `PROTOTYPE-START.md` en leg echte metingen vast in `commissioning-results.csv`. Alle fysieke tests staan bewust op NOT TESTED.

## Toegevoegd in L

Twee extra 5-polige ambientpoorten J28/J29 met afzonderlijk gezekerde 5 V voeding, voor externe PCA9615-adapters en twee SHT31-sensoren. Zie `REMOTE-SENSORS.md` voor de noodzakelijke vier adapters, 5 V sensorzijde, kabels en jumpers; deze modules zijn niet op de hoofdprint gemonteerd. De bestaande J26 blijft leeg bij twee remote SHT31's. Firmware L.1 meet beide sensoren en berekent hun dauwpunt.

## Afgerond

- TPS26630-ingang met inschakelstroomregeling, nominale 4,5 A hardwarebegrenzing, foutuitgang en PGOOD-voorwaarde voor het relais.
- Totale ingangsstroommeting op XIAO D1; aangepaste firmware-interface vastgelegd.
- XIAO-headeroriëntatie gecorrigeerd: componentzijde omhoog, USB naar de onderrand, J1 rechts en J2 links.
- Schema, stuklijst, footprints en routing bijgewerkt; vier TEC-kanalen en heater onderling vergrendeld.
- KiCad ERC: 0 fouten en 0 waarschuwingen. DRC: 0 overtredingen, 0 ontbrekende verbindingen, 0 footprintfouten.
- Schema/PCB-vergelijking: 202 componenten en 638 elektrische pinnen komen overeen. Kelvin- en BIAS-aftakkingen en massaverbinding gecontroleerd.
- Gerbers onafhankelijk teruggelezen: vier koperlagen, 423 PTH-boor-/sleufobjecten en 7 NPTH-objecten; booraantallen komen overeen met de bronprint.
- Schema-PDF, Gerbers, boorbestanden, BOM, plaatsingsdata, fabricage- en opstartinstructies toegevoegd.

## Grenzen van het eerste prototype

Mean Well GST160A24-R7B als 24 V voeding; begrens werkelijk totaalverbruik tot 4,0 A. Per TEC-kanaal maximaal 2,0 A gemeten, inclusief rimpel en overshoot. Heater en TEC niet tegelijk activeren. Begin glasproeven op 10 W en verhoog pas na temperatuur- en stroommetingen. Deze grenzen zijn proefvoorwaarden, geen bewezen thermische prestaties.

`firmware/README.md` beschrijft de meegeleverde testfirmware, flashprocedure en kalibratie. `FIRMWARE-INTERFACE.md` beschrijft de hardware-interface. Voor aangesloten glas en peltiers moeten foutafhandeling, kalibratie en uitschakeling op de echte print zijn getest. CAD-controles bewijzen geen regelstabiliteit, temperatuurstijging, EMC of veiligheid van de complete installatie.

## Ontwerp openen

Open `ito-controller.kicad_pro` met KiCad 9. De lokale symbool- en footprintbibliotheken zijn inbegrepen. Het schema is ook beschikbaar als `ito-controller-schema-rev-l.pdf`. `SHA256SUMS.txt` beschrijft de geleverde bestanden.

## Aanvulling L.1: prototypefirmware

Bewaakte testpulsen, een actieve vasthoudknop, ventilatortachbewaking, vijf sensoren, totale stroom en heaterstroom, I2C-timeouts, registercontrole, foutvergrendeling en watchdog zijn geïmplementeerd. De standaardbuild blokkeert vermogensvrijgave totdat meetwaarden zijn ingevuld. Eén TEC-kanaal per proef; automatische cycli en vierkanalenbedrijf zijn nog niet vrijgegeven. Revisie L voegt J28/J29, F4/F5 en C51/C52 toe, met bijgewerkte routing en fabricagebestanden.
