# ITO-controller L — eerste prototype

Deze revisie is bedoeld voor een kleine prototypebouw en stapsgewijze inbedrijfstelling. Gebruik uitsluitend schema, BOM en fabricagebestanden uit hetzelfde L-pakket. Er is nog geen fysieke print getest. De nulresultaten van CAD-controles gelden niet als thermische of EMC-kwalificatie.

## Vastgelegde uitvoering

- Glas: 100 × 100 × 1,1mm ITO, opgegeven 8Ω/□; twee 5mm-busbars. Nominaal 7,2Ω over de volledige busbars, te meten vóór aansluiten.
- Voeding: Mean Well GST160A24-R7B, 24V/6,67A/160W. Alleen deze 24V-uitvoering; geen netspanning op de PCB.
- Prototype: maximaal 4,0A werkelijke totale ingangsstroom; eFuse nominale grens circa 4,5A, tolerantie circa 4,18–4,82A. De adapterrating is geen PCB-rating.
- Vier TEC1-12706, maximaal 2,0A GEMETEN per kanaal. Begin bij 0,25A op één kanaal. Een ongekalibreerde DAC-instelling van 2A is niet toegestaan: regelaar- en shunttoleranties komen erbij.
- Heater en TEC niet tegelijk actief. TEC_ARM hoog blokkeert de heaterdriver hardwarematig. Firmware moet ook de reststroom afwachten.
- Ventilator: 24V, maximaal 0,5A inclusief aanloop binnen F3-karakteristiek; PWM/tach-interface volgens ASSEMBLY.md.
- Elektronica droog, omgevingslucht voor de eerste proeven 15–30°C. Geen condens op de PCB.

## Wat is in K afgesloten (overgenomen in L)?

1. De ongekwalificeerde langzame LTC4365/MOSFET-ingang is vervangen door TPS26630RGER met actieve inschakel- en overstroombegrenzing. MODE is open voor latch-off na de daarvoor gespecificeerde overbelastingsfout; niet alle foutsoorten blijven vergrendeld.
2. Q2 is nu IRLB4030PBF; Q3 is BSS138LT1G. Geen twee IRL540N's meer monteren. Q2: source naar +24_FUSED, drain naar EFUSE_IN.
3. Totale stroommeting toegevoegd op XIAO D1. De oude 3,3V-deler op D1 is vervallen.
4. U15 laat het relais alleen aantrekken bij POWER_ARM én eFuse PGOOD; R19 houdt de vrijgave laag tijdens reset.
5. Printconnectors, headerafstand, componentbestelnummers, koperopbouw en via-in-pad-proces zijn vastgelegd.
6. Gerbers, boorbestanden, stuklijst, posities en controleverslagen zijn opgenomen. Zie FABRICATION.md.

## Eerst inschakelen

1. Controleer na assemblage polariteit en oriëntatie. Meet geen vaste kortsluiting tussen +24_IN/GND, +24_PROT/GND, +5_AUX/GND en +3V3/GND; houd rekening met opladende condensatoren. Controleer met microscoop de vijf QFN's.
2. Laat glas, TEC's, fan en XIAO los. JP1 OPEN. Gebruik een laboratoriumvoeding op24V met aanvankelijk0,25A-limiet. R19/R21 moeten het relais uit houden. Verhoog alleen naar0,5A als de bulklaadstroom de labvoeding begrenst en er geen vaste fout is. Controleer +24_PROT en +5_AUX (5V binnen modulegegevens).
3. Plaats XIAO spanningsloos. Voed voor programmeren via USB met JP1 OPEN. Gebruik bij metingen een referentie naar GND: de adaptermin is aan protective earth gekoppeld. Plaats geen geaarde scoopklem op de zwevende gate/source- of shuntnodes; gebruik daarvoor een differentiële probe.
4. Programmeer de meegeleverde firmware volgens firmware/README.md. Deze is gecompileerd en heeft hosttests doorstaan, maar vereist nog kalibratie en hardwaretests. Controleer met losse belastingen dat reset, I²C-fout, afgebroken sensor en ontbrekende thermostaat geen vermogen vrijgeven. Sluit J6/J7/J8 met de bedoelde NC-veiligheidscontacten; alleen voor de bewaakte weerstandstest mogen testbruggen worden gebruikt, daarna verwijderen.
5. Test eerst de heateruitgang met circa10Ω/100W vermogensweerstand op een onbrandbare opstelling, begrensd op0,5A. Daarna één TEC-kanaal met circa4,7Ω/50W belasting. Verifieer stroominstelling, EN-uit, overgang CC/CV en overshoot voordat de stroom omhoog gaat.
6. Kalibreer totale ingangsstroom en heaterstroom bij twee bekende belastingen; vergelijk met een externe meter. Test eFuse-overbelasting met een elektronische belasting op+24_PROT, pas na de voorgaande controles. Bevestig latch-off/reset; gebruik geen losse draad als kortsluitproef.
7. Pas daarna Mean Well, koellichaam/fan en één echte TEC aansluiten. Monteer de warme kant vóór stroominschakeling. Verifieer de polariteit bij lage stroom. Voer de vierkanalenproef pas uit nadat één kanaal stabiel is en alle hardware-uitschakelingen werken.
8. Glas als laatste aansluiten. Meet de weerstand via de busbars, met een vierdraadsmeting als mogelijk. Start bij10W en korte pulsen, registreer alle glastemperaturen en hechting. Verhoog stapsgewijs naar20/40W en vervolgens de toegestane boost. Hanteer voor de eerste glasproeven een voorlopige softwarestop bij40°C en een onafhankelijke NC-overtemperatuurbeveiliging rond50°C. Dit kwalificeert coating/lijm/glas niet voor50°C; een lagere componentgrens gaat voor.

## Open na bestellen: fysieke testresultaten

De resterende punten zijn meetwerk aan het prototype: eFuse-hotplug, regelstabiliteit en rimpel, meetnauwkeurigheid, reset/foutgedrag, temperaturen van componenten/bedrading en snelheid van ontwasemen. Leg ze vast in commissioning-results.csv. Volledig vermogen en onbeheerd bedrijf zijn pas aan de orde nadat die proeven slagen.
