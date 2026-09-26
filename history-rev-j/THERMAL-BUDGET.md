# Thermisch rekenbudget — revisie I

Dit is een gevoeligheidsanalyse van het voorlopige 24V/6A-budget, geen meting of
vrijgegeven rating. Bestaande onderdeelgegevens en aannames staan in ENGINEERING-REVIEW.md.

## Ingangs-MOSFETs Q2/Q3

Met aangenomen warme RDS(on)=0,126Ω per IRL540NPBF is de dissipatie bij 6A
4,54W per MOSFET. Neem per onderdeel RθJC=1,1K/W, interface=0,5K/W en een
apart koellichaam van 10K/W: de berekende stijging is 4,536×11,6 = 52,6K.
Bij lokale koellucht van 40°C geeft dat circa 93°C junctie. Bij 60°C lucht circa
113°C. De echte RDS(on), interface, luchtstroming en montage moeten dit bevestigen.

Een gedeeld koellichaam moet voor dezelfde stijging ongeveer 5K/W zijn:
9,072W×5K/W plus 4,536W×1,6K/W = 52,6K. De drain-tabs hebben verschillende
netten: bij een gedeeld metalen koellichaam zijn elektrische isolatie en een
herberekening van de interfaceweerstand vereist. Koellucht uit de Peltier-heatsink
kan aanzienlijk warmer zijn dan kamerlucht.

Bij alleen de nominale 80W-heater is de ingangsstroom circa 3,33A en het verlies
in deze twee MOSFETs samen circa 2,80W volgens dezelfde warme-weerstandsaanname.
Dit omvat geen relais-, zekering-, connector-, heater-MOSFET- of koperverliezen.

## Vier TEC-kanalen

Vier belastingen van 12V×2A leveren 96W totaal. Bij een aangenomen rendement van
85% vraagt de conversie 112,9W en dissipeert zij 16,9W, circa 4,24W per kanaal.
Dit is een budget over IC, spoel en overige verliezen samen; geen voorspelling
van alleen de IC-dissipatie. Hulpvoedingen en de ingangs-MOSFETs komen erbij.
Bij 24V past dit globaal binnen 6A, maar bij lage ingangsspanning en ongunstige
verliezen is die marge kleiner. Het is geen bewijs voor volledig vermogen over
het hele ingangsbereik.

De 25mΩ-shunt dissipeert bij 2A gemiddeld 0,10W, exclusief rimpelbijdrage.
Voor een aangenomen warme spoel-DCR van 13mΩ is het DC-verlies circa 0,052W;
kernverlies en rimpelbijdrage komen erbij. Verdeel het resterende budget niet
willekeurig over componenten: meet rendement en temperatuur op het prototype.

## Meetpunten en besliscriteria

Meet lokale lucht, Q2/Q3-behuizing, LT8613-behuizingen, spoelen, shunts, C5,
zekeringen en connectoren bij heaterbedrijf en bij vier TEC-kanalen. Leg koper,
via-plating, koeling, omgevingstemperatuur en belastingsduur bij elk resultaat vast.
Een koude start alleen is onvoldoende: herhaal na thermisch evenwicht en bij
warme herstart. Schat junctietemperatuur alleen met een passend thermisch model;
behuizingstemperatuur is niet rechtstreeks junctietemperatuur.

De Peltier-heatsink moet Qhot = Qcold + Pelektrisch afvoeren. Zonder exact
TEC-model, koudebelasting en toegestane warmekanttemperatuur kan hiervoor geen
definitieve K/W-rating worden vastgesteld. Dit staat los van het bovenstaande
PCB-verliesbudget. Inschakel-SOA blijft open volgens INRUSH-REVIEW.md.

## Aanvulling revisie J

De BIAS-voeding is naar de eigen uitgangsrail verplaatst. Zie BIAS-REVIEW.md
voor topologie, rekenvoorbeelden en de nog vereiste fysieke verificatie.
