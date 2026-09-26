# Fabricage — ITO L prototype

Upload uitsluitend `manufacturing/ito-l-fabrication.zip` naar de printleverancier. Het volledige project-ZIP is voor ontwerp/assemblage. Bestel eerst een kleine serie, bijvoorbeeld5 kale printen; assembleer aanvankelijk1.

| Eigenschap | Bestelspecificatie |
|---|---|
| Afmeting / dikte | 160 × 110mm / 1,60mm ±10% |
| Materiaal | FR-4, Tg ≥150°C |
| Koperlagen | 4: F.Cu / In1.Cu / In2.Cu / B.Cu |
| Koper | Buiten nominale70µm afgewerkt, binnen35µm; geen automatische verlaging naar1oz buiten |
| Afwerking | ENIG |
| Masker / opdruk | Groen soldeermasker, witte opdruk |
| Minimale baan / koperafstand | 0,15 / 0,15mm; leverancier moet dit bij2oz buitenkoper ondersteunen |
| Kleinste via | 0,25mm afgewerkt boorgat; kleinste koperdiameter0,55mm |
| Via-in-pad | Epoxy vullen en koper afdekken (IPC-4761 type VII) voor vias in QFN exposed pads, zie via-in-pad.csv |
| Boorbestanden | PTH en NPTH gescheiden; maatvoering metrisch |
| Test | Elektrische nettest, AOI; geen impedantie-eis |
| Opbouw | Symmetrische standaard4-laagsopbouw van leverancier, totale dikte1,6mm en hierboven genoemd koper |

In1.Cu bevat massa én enkele routes: niet behandelen als een automatisch vol vlak. Het antennegebied moet op alle koperlagen leeg blijven. Gerbers zijn reeds met gevulde zones geëxporteerd; niet opnieuw vullen op basis van een andere regelset.

Laat de normale DFM-controle van de leverancier de opgegeven2oz/0,15mm-combinatie en via-in-pad bevestigen. Als de gekozen goedkope standaarddienst die combinatie niet accepteert, kies de passende fabricageoptie of een andere leverancier; vergroot geen spoorafstanden en verwijder geen vias op eigen initiatief.

Voor SMT: stencil100µm, originele F.Paste-data gebruiken. U1 heeft verdeelde pasteopeningen. Filled/capped exposed-pad-vias voorkomen soldeerwegvloeiing. Reflow en inspectie zijn nodig voor de QFN's en gestapelde uitgangscondensatoren. De centroidlijst geeft KiCad-hoeken; de assembler controleert zijn rotatieconventie aan de pin1-markeringen en montagetekening.

De meegeleverde CAD- en exportcontroles bewijzen aansluitingen en fabricagegeometrie, niet de werking van een gebouwde print.

## J30 R7B: gefreesde sleufgaten

J30 heeft vier **doorgemetalliseerde sleufgaten van 0,60 x 2,70 mm** en drie niet-doorgemetalliseerde montagegaten: twee rond 1,70 mm, één sleuf 2,20 x 1,00 mm. De maten zijn afgewerkte maten uit de Kycon-tekening. Behoud de sleuven in de Excellon-routegegevens; vervang ze niet door ronde gaten. Laat de leverancier de 0,60 mm gemetalliseerde sleuven en de afgewerkte tolerantie bevestigen bij DFM.
