# Prototypeproeven — nog niet uitgevoerd

Doel: het ontwerp op één kanaal onderbouwen voordat vier kanalen en het glas
op vol vermogen worden aangesloten. Een evaluatieprint of losse ingangstestcel
kan voor de eerste checks worden gebruikt; de definitieve layout moet later
zelf opnieuw worden gekwalificeerd. Leg meetapparatuur, probe-instellingen,
voeding, onderdelen, kabels, koeling en temperatuur vast bij elke registratie.

## A. Ingangstestcel

Gebruik LTC4365, Q2/Q3, gate-netwerk en dezelfde directe uitgangscapaciteit.
Hoofdlast aanvankelijk uit. Meet Q2 en Q3 elk met gelijktijdige VDS-, ID- en
VGS-kanalen; differentiële probes zijn nodig voor zwevende gate/source-metingen.
Een gewone geaarde scopeklem mag geen zwevend net kortsluiten.

1. Controleer polariteit en gate-off bij lage, begrensde energie.
2. Verhoog daarna naar het gekozen bereik: 20, 24 en 28V als ontwerptestpunten.
   Rond UV/OV kan schakelen juist geblokkeerd worden; overbrug die beveiliging niet.
3. Herhaal met 470µF nominale en 600µF scenario-capaciteit, hulpvoeding uit/aan,
   ventilatorstart, koude en warme herstart. Leg iedere echte capaciteit vast.
4. Meet bronstroombegrenzing/hiccup en registreer de volledige ramp en herhalingen.
5. Beoordeel de VDS/ID-trajecten en casetemperatuur tegen geldige SOA-informatie
   voor pulsduur en herhaling. Geen extrapolatie voorbij beschikbare curves.

Acceptatie: aantoonbare SOA-marge op alle bedoelde bedrijfsgevallen, gate-spanning
binnen onderdelenlimieten, geen ongecontroleerde oscillatie of herstartcyclus.
Zolang temperatuur-/pulsdekking ontbreekt blijft dit punt BLOCKED, ook bij overleven
van enkele proefstarts. Een fan of actieve converter kan de zuiver capacitieve ramp wijzigen.

## B. Eén TEC-regelaar

Begin met een gekoelde weerstand of elektronische belasting met geschikt bereik.
Voorbeelden: 3Ω voor CC bij maximaal 2A (12W) en 10Ω voor CV rond 12V (circa15W).
Kies vermogensmarge en koeling passend bij de echte belasting. Gebruik eerst
beperkte stroom; sluit het ITO-glas nog niet op de heater aan.

Meet VOUT lokaal én bij de belasting, werkelijke belastingstroom, ICTRL, ISP−ISN,
SW en VIN. Gebruik korte probeverbindingen en controleer meetartefacten.

- VIN: 20, 24 en 28V, voor zover de beveiligingen normaal vrijgeven.
- Stroomopdracht: nul, laag, 0,5A, 1A, 1,5A en doelmaximum; registreer gemeten stroom.
- Staptesten: 0,5↔1,5A in CC, belastingssprongen in CV en CC/CV-overgangen.
- Kabels: korte referentiekabel en de werkelijke TEC-kabel.
- Opstart/uitschakeling: EN, DAC-nul, MCU-reset, brownout en verbreken van sensoren.
- Open belasting eerst met lage energie; kortsluitproeven pas na normale werking,
  met passende begrenzing en registratie. Herhaal niet blind bij hiccup of opwarming.

Voorlopige interne ontwerpdoelen (nog af te stemmen op TEC en proces): geen groeiende
of aanhoudende regellusoscillatie, staprespons binnen 5% van eindwaarde na 10ms,
geen stroom- of spanningspiek boven de goedgekeurde belastingslimiet. Burst-rimpel
bij lichte last apart beoordelen. Leg absolute pieken vast; alleen procentuele
overshoot rapporteren is bij lage stroom misleidend.

Voor een echte frequentieresponsmeting: laat injectiepunt en methode controleren
voor zowel CV als CC. Interne streefwaarden zijn ≥45° fasemarge en ≥6dB gainmarge;
een nette staprespons alleen bewijst deze waarden niet. Een fabrikantmodel kan
voor screening helpen maar vervangt geen meting aan layout, componenten en kabels.

## C. Warmte en vier kanalen

Meet eerst één kanaal op stationaire belasting. Log vervolgens Q2/Q3, LT8613's,
spoelen, shunts, zekeringen, C5, connectors en lokale koellucht bij alle vier kanalen.
Thermokoppels op elektrisch actieve tabs isoleren; IR-metingen voor emissiviteit
corrigeren. Beschouw een drift <1°C per 10 minuten gedurende twee intervallen
als praktisch evenwichtscriterium, niet als universele thermische norm.

Start bij kamertemperatuur en herhaal bij de gekozen maximale lokale omgeving.
Gebruik voor eerste proeven 40°C als gepland punt, niet als vastgestelde productrating.
Voorlopige eigen ontwerpdoelen: MOSFET-juncties ≤125°C en LT8613-juncties ≤110°C,
beide met aantoonbare schattingsmarge. Alle passieve delen blijven onder hun eigen
ratings met ontwerpmarge. θJA uit een referentieprint niet klakkeloos op deze PCB
 toepassen; behuizingstemperatuur is niet de junctietemperatuur.

Vierkanaalsbedrijf eerst bij 24V of hoger en met externe totale stroommeting.
Stel het maximale continue vermogen vast inclusief fan en hulpvoeding. Leg daarna
pas een firmwarelimiet/deratingcurve vast. Test heaterbedrijf afzonderlijk en
verifieer dat heater en actieve TEC-aansturing elkaar uitsluiten.

## Ontbrekende gegevens die geen meetplan kan vervangen

Exacte TEC-, voeding- en ventilatormodellen, maximaal toegestane glas/coating-
temperatuur, koelluchtcondities, PCB-stackup en montage van koellichamen.
Zonder deze gegevens zijn absolute productgrenzen en definitieve vrijgave onbepaald.

## Extra proef voor revisie J

Doorloop in CC-bedrijf de uitgangsspanning van0–4V en terug, inclusief0–3,3V,
om de interne overgang tussen VIN- en BIAS-voeding te controleren. Meet de
uitgang, BIAS, INTVCC en belastingstroom tegelijk; herhaal met werkelijke kabels.
