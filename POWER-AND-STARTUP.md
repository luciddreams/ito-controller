# Vermogen en inschakelen K

R14=4,02kΩ0,1% stelt de TPS26630 rond4,5A in; werkelijke datasheetband circa4,18–4,82A. Voor de eerste print geldt maximaal4,0A werkelijke bedrijfsstroom. Geen gelijktijdige80W heater en vier TEC's.

UV-deler156k/10k:19,92V nominaal. OV223k/10k:27,96V. PGOOD150k/10k:19,2V nominaal én interne FET moet volledig worden aangestuurd. Deze drempels hebben tolerantie; ze zijn geen precisiespanningsklemmen.

C4 is een bestelbare100nF/50V-X7R, geen veronderstelde idealeC0G. Het model rekent met70–127nF effectief, met marge voor beginwaarde, temperatuur en veroudering. Bij22,8–28V en470–600µF uitgangsbulk komt de berekende capacitieve ramp uit op 27.6–85.3ms en maximaal 0.496A capacitieve laadstroom. Hulpvoeding en fan komen erbij; thermische regeling kan de werkelijke opstart verlengen. PGOOD blokkeert het relais tot de uitgang gereed is. Meet de werkelijke hotplug en warme herstart; dit is geen gemeten opstarttijd.

De interne eFuse neemt de capacitieve-rampbelasting over van de oude discrete ingangstrap. Q2 is alleen de reverse-block FET. Met4,5mΩmax bij25°C is zijn geleidingsverlies bij4A0,072W; met een expliciete2×warmweerstandsaanname0,144W. U1 kan bij53mΩ en4A ongeveer0,85W geleidingsverlies hebben en moet via zijn exposed pad en massakoper warmte afvoeren.

De glasberekening24²/7,2=80W is ideaal. Spanningsval door adapterkabel, eFuse, relais, shunt en PCB verlaagt dat. Meet de weerstand via de gemonteerde busbars: een puntmeting op het ITO levert door contact-/spreidingsweerstand geen bruikbare totale heaterweerstand op. Bij een te lage weerstand lost alleenPWM de piekstroom niet op.

VierTEC's op werkelijk12V/2A zouden96W aan de uitgangen vragen en passen inclusief verliezen en hulpverbruik niet in dit4A-prototypebudget. Dat is geen toegestane gelijktijdige instelling. Bij2A kan eenTEC1-12706 in werkelijkheid beduidend minder dan12V vragen, afhankelijk van temperatuurverschil; meet dit. De totale stroommeting begrenst de gezamenlijke vraag.

De0,15mm-meet-/besturingsbanen zijn niet bedoeld als4A-pad. De hoofdingang is opnieuw met2mm-routes gelegd, met alleen zeer korte0,24mm-uitwaaiering per parallelle eFuse-powerpin. Gebruik70µm buitenkoper,35µm binnenkoper zoals opgegeven. Het rapport power-path-review.json controleert geometrische verbindingen; het is geen IPC-2152-ampacitycertificaat. Meet spanningsval en lokale temperatuur tijdens de stroomtrap.

Voor de eerste thermische proef: stop bij componentbehuizing80°C, connector/bedrading60°C of een onverklaarde temperatuurstijging; blijf ook onder strengere fabrikantgrenzen. Deze conservatieve teststop is geen afgeleide maximale junctietemperatuur. Start op0,5A en verhoog stapsgewijs.

F1=10A is aanvullende foutbeveiliging. De6,67A-adapter kan hem bij een fout heel laten. De primaire bedrijfsbegrenzing komt vanU1; F1 is niet de4A-beveiliging. Een interne fout vóórU1 valt onder bronbeveiliging, bedrading en de bewaakte prototypeopstelling.
