# Kelvin-review — revisie I

## Uitgevoerd

De oude ISP/ISN-aftakkingen zijn verwijderd. U11–U14 pin 25 en 26 hebben nu elk
een eigen 0,15mm-spoor naar respectievelijk pad 1 en 2 van RS1–RS4.
U3 pin 10 (IN+) en pin 9 (IN−) gaan afzonderlijk naar RSH1 pad 1 en 2.
Pin 8 (VBUS) heeft een derde, afzonderlijke verbinding naar RSH1 pad 2.

Routing behandelde ook koper op hetzelfde net als obstakel, behalve binnen de
bronpad en de bedoelde shuntpad. Daarmee wordt een onbedoelde aansluiting op een
nabije vermogensbaan voorkomen. Nieuwe meetroutes gebruiken geen In1.Cu-sporen;
through-vias gaan uiteraard door het bord en hebben plane-clearances.

## Controle

`kelvin-isolation-review.json` bevat elf aansluitingen. Voor elke aansluiting:

1. Koperpolygonen uit het werkelijke PCB-bestand geëxporteerd.
2. De bedoelde shuntpad virtueel uit het koper verwijderd, met 3µm snijmarge.
3. De verbonden kopercomponent vanaf de meetpin gevolgd, inclusief laagwisselingen.
4. Getoetst dat geen andere componentpad bereikbaar blijft.

Alle elf aansluitingen slagen. Native DRC bevestigt daarnaast dat ze in het
volledige ontwerp daadwerkelijk aangesloten zijn. Samen toont dit de bedoelde
aparte aftakking tot aan de shuntpad aan. Het is geen elektrisch veldmodel.

## Grenzen

De shunts zijn nu WSL2512R0250FEA en WSL2512R0100FEA, met het fabrikantlandpattern
voor deze weerstandswaarden. De elf routes zijn opnieuw gelegd via een venster
van 0,65mm aan de binnenzijde van iedere shuntpad. Zo benaderen de meetlijnen
de kant van het weerstandselement. De eindpunten liggen 2,355mm van het midden
van de shunt. Kelvin naar een tweepolige PCB-pad elimineert niet alle soldeer- en
terminalweerstand: absolute nauwkeurigheid moet nog gemeten worden.
De meetparen zijn niet als gematchte differentiële impedantieparen gerouteerd.
Controle van lusoppervlak, SW-koppeling, retourvlak en gemeten ruis blijft nodig.
Bij wijzigingen aan shuntfootprints moet deze geometriecontrole opnieuw worden uitgevoerd.

Bronnen:
- https://www.analog.com/media/en/technical-documentation/data-sheets/lt8613.pdf
- https://www.ti.com/lit/ds/symlink/ina226.pdf

## Revisie I

De grotere KPS-uitgangscaps hebben nieuwe 1,2mm-vermogensroutes.
De elf afzonderlijke meettakken zijn opnieuw geometrisch gecontroleerd.
Zie de opnieuw gegenereerde JSON-rapporten voor deze revisie.

## Aanvulling revisie J

De BIAS-voeding is naar de eigen uitgangsrail verplaatst. Zie BIAS-REVIEW.md
voor topologie, rekenvoorbeelden en de nog vereiste fysieke verificatie.
