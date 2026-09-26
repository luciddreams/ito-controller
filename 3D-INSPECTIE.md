> J30 R7B-printbus toegevoegd; J3 en J11-J14 180 graden gedraaid. Zie `R7B-UPDATE.md`. J30 gebruikt een eigen vereenvoudigd STEP-maatmodel op basis van Kycon A13, inclusief neusoverhang en montagepennen.

# 3D-inspectie — ITO-controller L

## Status — plaatsingscorrectie 24 september 2026

C5 verplaatst: pin 1 van (15,60; 79,10) naar (12,00; 101,75) mm. Het middelpunt van de behuizing ligt op (14,50; 101,75) mm. De weerstanden naast de XIAO staan op een regelmatig raster; 17 weerstanden zijn verplaatst. Zie `PLACEMENT-UPDATE.md`.

De eerdere modeloverlappingen C5/Q2, C5/F1 en C5/Q5 zijn opgelost. De herhaalde STEP-intersectiecontrole meldt geen componentoverlap boven de gebruikte 0,05 mm³ rapportagegrens. De fabricageblokkade voor deze plaatsing is opgeheven. Alle voorwaarden voor de eerste prototypebouw, kalibratie en fysieke tests blijven gelden.

## Openen

1. Pak het volledige ZIP uit, inclusief `3dmodels` en `ITO_PCB.pretty`.
2. Open `ito-controller.kicad_pro` in KiCad 9, daarna de PCB-editor.
3. Open **View / Beeld → 3D Viewer** (Alt+3). Toon zowel THT- als SMD-modellen.
4. Modellen zijn gekoppeld via `${KIPRJMOD}/3dmodels/...step`; geen systeeminstallatie van modelbibliotheken nodig.
5. J1 bevat twee modellen: zijn socket en de vereenvoudigde XIAO-opstelling. Schakel het XIAO-model via footprint-eigenschappen → 3D-modellen uit om onder de module te kijken. Het modulemodel is bewust alleen aan de geplaatste J1 gekoppeld, niet aan de generieke socketbibliotheek.

`3d-review/isometric.png`, `top.png` en `bottom.png` zijn echte KiCad-renders. `3d-review/ito-controller-rev-l.step` is een export met componenten voor een mechanische CAD-viewer.

## Dekking en nauwkeurigheid

Alle **202 elektrische componentposities** hebben een lokaal STEP-model. De 4 montagegaten en 3 fiducials zijn printgeometrie en hebben geen los componentmodel nodig. Daarnaast is de ingeplugde XIAO als vereenvoudigd montagemodel opgenomen.

De standaardbehuizingen komen uit de officiële KiCad-modelbibliotheek. Het zijn behuizingsmodellen, geen gecertificeerde modellen van ieder specifiek MPN. Pin-1/origin en rotatie zijn op footprintconventies gebaseerd en visueel beoordeeld; echte onderdelen blijven leidend. Controleer polariteit altijd ook aan schema, BOM en datasheet.

| Eigen vereenvoudigd model | Aannamen / beperking |
|---|---|
| C5 | Ø12,5 mm, hart 2,5 mm vanaf pin 1, pinsteek 5 mm, hoogte-reservering 22 mm. Geen exacte bodem/ventielvorm; vervangt het te lage generieke 12,5 mm model. |
| K1 G2RL-1A-E | Buitencontour 29 × 12,7 × 15,7 mm, 6 pennen op het bestaande raster; geen interne details. Omron G2RL-tekening. |
| L1–L4 IHLP-6767 | Vereenvoudigde 17,15 × 17,15 mm behuizing, circa 12,55 mm totale modelhoogte. Familievorm; exacte GZEK100M11-uitvoering en toleranties nog mechanisch bevestigen. |
| KPS-condensatoren | 6 × 5 × 5 mm nominale dubbele stapel; echte maxima volgens geraadpleegde KPS-tabel 6,5 × 5,5 × 5,5 mm. Reserveer die maxima plus speling. |
| Littelfuse 451 | Vereenvoudigde 6,1 × 2,69 × 2,69 mm behuizing met eindkappen. Geen fabrikant-CAD; eindkapdetails benaderd. |
| J1/J2 sockets | 2,54 mm raster, 8,51 mm familiehoogte; vereenvoudigde gaten en pennen. |
| XIAO | Oranje montagemodel, print 17,8 × 21 mm. Aangenomen socket 8,51 mm + mannelijke headerafstandhouder 2,54 mm + print 1 mm. USB en componentvolume zijn benaderingen. Werkelijke hoogte hangt af van de gekozen mannelijke headers en insteekdiepte. Geen fabrikantmodel. |
| WSL-shunts | Generiek KiCad 2512-model; geen exacte terminalvorm van de Vishay-shunt. |

Bedrading, aangesloten stekkers, kabelbuigradius, schroevendraaiertoegang, externe sensormodules en eventuele koellichamen zijn niet gemodelleerd. Geen enkele afbeelding vormt een definitieve mechanische vrijgave.

## Controle

- C5 en 17 weerstanden verplaatst; routing hersteld. Nets, waarden en componentselectie ongewijzigd. C5 heeft nu ook een fysieke courtyard.
- KiCad DRC na toevoegen: 0 overtredingen, 0 open verbindingen, 0 footprintafwijkingen.
- Alle gekoppelde STEP-bestanden bestaan en zijn ingelezen.
- STEP-solid-intersecties opnieuw gecontroleerd; geen ongewenste componentintersecties gerapporteerd. Bedoelde passing tussen XIAO en sockets uitgesloten. Dit is geen tolerantiesimulatie of volledige clearancecontrole.
- Gerbers, boorbestanden, componentposities, STEP-export en renders opnieuw gegenereerd uit de gecorrigeerde print. `manufacturing/export-validation.json` vermeldt de actuele bronhash.

## Bronnen en hergebruik

KiCad: https://github.com/KiCad/kicad-packages3D — per model de downloadbron en SHA256 in `3dmodels/sources.json`. Licentie/attributie: `3dmodels/KICAD-LICENSE.md`; bibliotheekmodellen CC-BY-SA 4.0 met KiCad-designuitzondering.

Eigen vereenvoudigde modellen: gegenereerd voor dit project, reproduceerbaar met `3dmodels/generate-custom-models.py` en CadQuery. Geen fabrikantgoedkeuring.

Mechanische referenties:
- Omron G2RL: reeds geraadpleegde G2RL-datasheet, afmetingen G2RL-1A-E.
- KEMET/Yageo KPS X7R C1129, 04/26, pagina 3 (dubbele 2220).
- Seeed: https://wiki.seeedstudio.com/XIAO_BLE/ — module 21 × 17,8 mm.
- Samtec: https://suddendocs.samtec.com/prints/ssw-1xx-xx-xxx-x-xx-xxx-xx-mkt.pdf — SSW-familietekening.
- Vishay: https://www.vishay.com/docs/34310/ihlp6767gz-1a.pdf — GZ-familie als benadering, niet de exacte bestelde MPN.
