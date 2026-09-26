# Inschakelreview — revisie I

C4 blijft 10nF. Er is geen onbewezen wijziging aan de gate-ramp doorgevoerd.
Deze review werkt de keuze en benodigde validatie verder uit.

## Eigen hoekberekening

Aannames: 28V, maximaal 600µF direct op +24_PROT, Cgate ±10%, laadstroom
12–30µA. Relais/loadbus moet tijdens de eerste ramp uit blijven.
t = V × Cgate / Igate; Icap = Cout × Igate / Cgate.

| Cgate (nF) | Berekende ramptijd (ms) | Maximale capacitieve stroom (A) |
|---:|---:|---:|
| 3.3 | 2.77–8.47 | 6.06 |
| 4.7 | 3.95–12.06 | 4.26 |
| 10 | 8.40–25.67 | 2.00 |
| 22 | 18.48–56.47 | 0.91 |

De kortste tijd en hoogste stroom horen bij dezelfde snelle hoek; de langste tijd
hoort bij een lagere stroom. Actieve belasting van hulpvoeding en ventilator komt
erbij. 600µF is een ontwerpaannname, geen gemeten effectieve capaciteit.
Gate-lading, plateau, componenttoleranties en bedrading zijn niet volledig gemodelleerd.

Een kleinere condensator reduceert de pulsduur maar vergroot de piekstroom.
De 10ms-SOA-curve van IRL540N kwalificeert niet automatisch een ramp van 25,7ms,
en een snelle ramp onder 10ms is evenmin automatisch veilig bij de hogere stroom.
De koude SOA-curve is geen bewijs voor herstart bij hete behuizing.

## Concrete vervolgstap vóór vrijgave

Gebruik een aparte ingangstestopstelling met dezelfde Q2/Q3, LTC4365, C4,
voedingsmodule en ingangscondensatoren. De definitieve print is hiervoor niet nodig.

- Meet tegelijk VDS en ID van elk belast transistorpad tijdens inschakelen;
  gebruik een geschikte differentiële spanningsprobe en stroomprobe/shunt.
- Leg VGS vast; controleer oscillatie, hoogste spanning en volledig inschakelen.
- Herhaal bij hoogste toegestane voedingsspanning, maximale belastingcapaciteit,
  hoogste starttemperatuur en herhaald aan/uit. Laat de hoofdlast eerst uit.
- Test vervolgens activeren van het relais, fanstart, onderspanning en overspanning.
- Beoordeel de volledige tijdafhankelijke trajecten tegen fabrikant-SOA inclusief
  temperatuur en herhalingsgedrag. Alleen totaalenergie of piekstroom toetsen is onvoldoende.
- Wanneer geen verdedigbare marge bestaat: kies een MOSFET met gegarandeerde
  lineaire SOA voor deze pulsduur of herontwerp naar een hot-swap-regelaar met
  expliciete stroombegrenzing. Niet alleen C4 verder verkleinen.

Benodigde invoer: voedingsmodel en stroombegrenzing, werkelijk direct aangesloten
capaciteiten, inschakelgedrag M1/fan en maximale temperatuur van het koellichaam.

Bronnen:
- https://www.analog.com/media/en/technical-documentation/data-sheets/ltc4365.pdf
- https://www.infineon.com/assets/row/public/documents/24/49/infineon-irl540n-datasheet-en.pdf
