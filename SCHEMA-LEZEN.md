# Schema lezen — revisie L, 25 september 2026

Open `ito-controller.kicad_pro` in KiCad 9 en vervolgens de schema-editor. Het hoofdblad geeft toegang tot de 13 functionele schema­bladen. De PDF bevat hetzelfde schema op 14 liggende A3-pagina’s; het is een vectorbestand en blijft scherp bij inzoomen.

## Indeling

1. Overzicht met de functionele bladen.
2. MCU, 5 V en USB.
3. R7B-ingang en ingangsbeveiliging.
4. Relais en hardware-uitschakeling.
5. ITO-heater.
6. DAC en stroominstellingen.
7. Vrijgave van de Peltierkanalen.
8–11. Vier identiek getekende Peltier-stroomregelaars.
12. Temperatuurmeting van het glas.
13. Koelblok, luchtvochtigheid en ventilator.
14. Twee externe ambientpoorten.

## Tekenconventies

De signaal- en energiestroom loopt overwegend van links naar rechts. Componenten zijn met echte KiCad-draden verbonden; een stip markeert een aftakking. Een kruising zonder stip is geen verbinding. Massa gebruikt de gebruikelijke GND-symbolen. Voedingsrails zijn binnen ieder blad bedraad; gelijknamige voedingssymbolen verbinden de bladen.

Globale netlabels tonen de oorspronkelijke netnamen en verbinden gelijknamige netten over de bladen heen. Ook interne netten behouden hun namen voor vergelijking met de bestaande PCB. Ze staan daarom soms tweemaal op dezelfde bedrade verbinding. Blauwe kruisjes markeren bewust niet-aangesloten pinnen.

Meerdere fysieke IC-pinnen op hetzelfde aansluitpunt zijn gestapeld: onder andere VIN, SW en GND van de LT8613 en IN/OUT/GND van de TPS26630. Een notitie op het betreffende blad noemt alle pinnummers. Ook de parallelle relaiscontacten zijn benoemd. Alle fysieke pinnen blijven aanwezig in de netlijst.

Referenties zijn normale bewerkbare KiCad-referentievelden. Voor een compacte tekening staat de zichtbare waarde in het veld `Schematische waarde`. Het oorspronkelijke volledige `Value`-veld is verborgen maar behouden, samen met footprint, fabrikant en onderdeelnummer. Bij een latere waardewijziging moeten beide waardevelden worden bijgewerkt.

## Controle

Alle 202 fysieke componenten en 638 elektrische pinnen zijn vergeleken met de oorspronkelijke netlijst: netnamen, verbindingen, niet-aangesloten pinnen, waarden, footprints en componentidentiteiten zijn gelijk. KiCad ERC: 0 meldingen. Zie `schematic-redraw-validation.json` en `schematic-erc.rpt`.

Deze wijziging betreft de schematekening. PCB, fabricagebestanden en firmware zijn niet gewijzigd. De eerdere beperkingen en inbedrijfstellingsinstructies voor het prototype blijven van toepassing.
