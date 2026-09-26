# R7B-printingang en connectorcorrectie - 24 september 2026

J30 is toegevoegd als directe aansluiting voor de Mean Well GST160A24-R7B: Kycon KPJX-4S, vierpolig, vergrendelbaar, through-hole. Beide positieve contacten (1/4) en beide retourcontacten (2/3) zijn aangesloten. J3 blijft als alternatieve ingang; nooit twee voedingen aansluiten.

J30 staat aan de onderrand, opening naar buiten. Footprintorigin/frontcentrum = (82,114) mm, hoek 180 graden; de neus steekt 4 mm buiten de 160 x 110 mm print. De behuizing moet ruimte bieden voor stekker en vergrendeling. Het meegeleverde STEP-model is een vereenvoudigd maatmodel, geen fabrikantmodel; vergrendelingskracht en passing moeten met het bestelde onderdeel worden gecontroleerd.

De Kycon A13-tekening geeft een onderaanzicht. De lokale footprint heeft pin1 op (+2.90,14.65), pin2 op (-2.90,14.65), pin3 op (+2.50,11.00), pin4 op (-2.50,11.00) mm, met de opening in -Y. De vier contactgaten zijn 0.60 x 2.70 mm gemetalliseerde sleuven. Zie FABRICATION.md.

De eerdere correctie van J3 en J11-J14 is meegenomen: 180 graden gedraaid om het midden tussen de twee pennen, met behouden pinnummers/netten, herstelde routing en + / - opdruk. J11-J14 openen naar de bovenrand; J3 naar de vrije ruimte boven de klem. C5 en het uitgelijnde weerstandencluster blijven op de gecorrigeerde plaatsen.

Schema, stuklijst, pinout, lokale bibliotheken en 3D-model zijn bijgewerkt. Firmware blijft L.1. De prototypegrens blijft 4 A totaal en 2 A gemeten per TEC-kanaal; deze wijziging kwalificeert geen hoger vermogen.

Bronnen:
- Kycon KPJX-4S, tekening A13: https://www.kycon.com/Pub_Eng_Draw/KPJX-4S.pdf
- Mean Well GST160A: https://www.meanwell.com/Upload/PDF/GST160A/GST160A-SPEC.PDF
- Mean Well verloopselectie: https://www.meanwell.com/upload/pdf/DC_plug.pdf

Validatie: KiCad ERC/DRC 0 overtredingen, 0 onverbonden pads, 0 footprintfouten; 202 componenten en 638 pinnen gelijk tussen schema en PCB; één verbonden massanet; 11 Kelvin-aftakkingen en 4 BIAS-aftakkingen behouden. De J30-ingangsroute is minimaal 2,0 mm op de 70 micrometer buitenlaag, zonder extra voedingsvias. De bestaande J3-F1 ingang bevat eveneens 2,0 mm banen; TEC-uitgangen blijven minimaal 1,2 mm. 7442 segmenten, 323 vias. Gerbers/Excellon onafhankelijk teruggelezen: 423 PTH-objecten, 7 NPTH-objecten. Fysieke metingen blijven vereist.
