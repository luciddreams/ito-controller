# Power budget and startup — revision K design retained in L

R14 = 4.02 kΩ, 0.1%, sets TPS26630 current limiting to approximately 4.5 A. The calculated datasheet range is approximately 4.18–4.82 A. Limit actual operating current of the first board to 4.0 A. Do not run an 80 W heater together with four TECs.

The 156 kΩ/10 kΩ UV divider gives 19.92 V nominal; 223 kΩ/10 kΩ OV gives 27.96 V; 150 kΩ/10 kΩ PGOOD gives 19.2 V nominal and also requires the internal FET to be fully enhanced. These thresholds have tolerances and are not precision voltage clamps.

C4 is an orderable 100 nF / 50 V X7R part, not an assumed ideal C0G capacitor. The model uses 70–127 nF effective capacitance, allowing for initial tolerance, temperature and aging. With 22.8–28 V input and 470–600 µF output bulk, the calculated capacitive ramp is 27.6–85.3 ms and capacitive charging current is at most 0.496 A. Auxiliary loads and the fan add current; thermal regulation may extend startup. PGOOD inhibits K1 until the output is ready. Verify actual hotplug and warm restart; these are calculations, not measurements.

The internal eFuse handles capacitive-ramp stress previously assigned to the discrete input stage. Q2 is only the reverse-block FET. At 4.5 mΩ maximum at 25°C, its calculated loss at 4 A is 0.072 W, or 0.144 W using an explicit 2× hot-resistance assumption. U1 may dissipate approximately 0.85 W at 53 mΩ and 4 A; its exposed pad and ground copper must remove this heat.

The glass calculation 24²/7.2 = 80 W is ideal. Cable, eFuse, relay, shunt and PCB voltage drops reduce actual heater power. Measure resistance between the installed full busbars: point-contact readings include contact/spreading resistance and are not the correct total heater resistance. PWM alone cannot correct excessive on-state current caused by resistance that is too low.

Four TECs actually drawing 12 V / 2 A would require 96 W at their outputs and exceed the 4 A input budget once losses and auxiliary consumption are included. This is not an allowed simultaneous setting. A TEC1-12706 at 2 A may require considerably less than 12 V depending on temperature difference; measure it. Total input-current control overrides cooling demand.

The 0.15 mm measurement/control tracks are not 4 A paths. The main input uses 2 mm tracks, with only very short 0.24 mm fanout at each parallel eFuse power pin. Use specified 70 µm outer and 35 µm inner copper. `power-path-review.json` checks geometry/connectivity; it is not an IPC-2152 ampacity certificate. Measure voltage drop and local temperature during current steps.

For initial thermal tests stop at 80°C component case temperature, 60°C connectors/wiring, or an unexplained temperature rise; stricter manufacturer limits take precedence. These are conservative test stops, not derived maximum junction temperatures. Start at 0.5 A and increase gradually.

F1 = 10 A is supplemental fault protection. The 6.67 A adapter may not clear it during a fault. U1 provides primary operating-current limiting; F1 is not the 4 A protection. Faults ahead of U1 depend on source protection, wiring and the supervised prototype setup.
