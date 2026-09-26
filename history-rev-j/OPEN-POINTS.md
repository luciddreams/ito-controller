# Open punten — revisie J

| Punt | Status / concreet vervolg |
|---|---|
| Q2/Q3, M1, L1–L4 selecteren | Bestelnummers opgenomen; elektrische/pinoutreview uitgevoerd; praktijkkwalificatie open |
| Lokale VIN- en bootstraprouting | Herlegd in alle vier kanalen; native DRC schoon; pulslus/retourpadreview nog nodig |
| Gelijktijdige heater/TEC-aansturing | Hardware-interlock toegevoegd; functietest bij reset, brownout en omschakelen open |
| Kelvin-meetverbindingen | Topologie hersteld: 8 TEC- en 2 heaterstroommeetlijnen rechtstreeks aan shuntpads, plus aparte VBUS-lijn. Geometrische isolatiecontrole geslaagd. Meetruis en shuntlandpattern nog kwalificeren |
| Inschakel-SOA Q2/Q3 | Niet gesloten: langzame hoek tot circa 26ms; beschikbare 10ms-SOA-curve bewijst die hoek niet. Hot-start meten en/of gate-ramp herontwerpen |
| Zekeringen en connectors | Zekeringen: Littelfuse451-MPN en landpattern gekozen; bronkortsluitstroom, pulscoördinatie en definitieve connectors blijven open |
| Condensatoren | Acht ingangscaps en C5 hebben nu MPN. Acht uitgangscaps nu KEMET KPS, fabrikantpads en typische DC-bias verwerkt; gegarandeerde effectieve capaciteit, ingangsbias, rimpelstroom en regelstabiliteit blijven open |
| Shunts | WSL2512R0250FEA / WSL2512R0100FEA en fabrikantpadmaten vastgelegd. Meetnauwkeurigheid, temperatuur en soldeerproces nog testen |
| Stackup en warmte | 70µm buiten/35µm binnen voorstel; thermisch rekenbudget toegevoegd in THERMAL-BUDGET.md; fabriek moet stackup, via-plating en thermisch ontwerp bevestigen |
| Ventilatorroute | Extra 0,8mm-pad M1:+24_PROT → F3 toegevoegd. Geometrische vernauwing opgelost; echte fanstroom, zekering en warmte blijven te bevestigen |
| Mechanica | XIAO/headerafstand, USB-toegang, TO-220-koellichamen en montage met echte onderdelen controleren |
| Systeemtest | Eén TEC-kanaal eerst: stroomstappen, overshoot, kortsluiting/open belasting, thermische afschakeling, sensorbreuk; daarna vier tegelijk |

Dit zijn vrijgaveblokkades; nul ERC/DRC-fouten maakt de print niet productierijp.
Benodigde externe gegevens: exact Peltiermodel, voedingsmodel/stroombegrenzing,
ventilatorgegevens en gekozen PCB-fabrikant/stackup. Er is geen specifieke Peltier
of voedingskarakteristiek aangenomen als reeds bevestigd.

## Bevindingen van de vervolgverificatie

- Viermaal12V/2A past bij20V niet binnen het voorlopige6A-budget. De print meet
  totale ingangsstroom niet rechtstreeks; een gevalideerde begrenzing blijft nodig.
- Toleranties van CV/CC zijn doorgerekend: circa11,55–12,33V spanningsscreening;
  circa2,26A bij een expliciete sense-testhoek, geen universele maximumgarantie.
- BIAS van alle LT8613-regelaars is in J verplaatst naar de eigen uitgangsrail;
  verifieer de bronwisseling rond3,1V en meet de verliesreductie.
- Concrete teststappen en nog lege resultatenlijst staan onder `qualification/`.
