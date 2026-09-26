# ITO controller L — first prototype commissioning

Use only schematic, BOM and manufacturing files from the same revision-L package. No physical board has been tested. Zero CAD violations do not constitute thermal or EMC qualification.

## Defined implementation

- Glass: 100 × 100 × 1.1 mm ITO, specified 8 Ω/□, two 5 mm busbars. The design estimate is 7.2 Ω between full busbars; measure before connecting.
- Supply: Mean Well GST160A24-R7B, 24 V / 6.67 A / 160 W. Use only this 24 V version; no mains voltage on the PCB.
- Prototype: actual total input current ≤4.0 A. eFuse nominal limit approximately 4.5 A, calculated tolerance approximately 4.18–4.82 A. Adapter capacity is not the PCB rating.
- Four TEC1-12706 modules, each limited to ≤2.0 A **measured**. Start with one channel at 0.25 A. An uncalibrated nominal 2 A DAC setting is not acceptable because regulator and shunt tolerances apply.
- Heater and TECs must not operate together. TEC_ARM high blocks the heater driver in hardware. Firmware must also wait for residual current to decay.
- Fan: 24 V, ≤0.5 A, including startup within F3's characteristic; PWM/tach wiring per ASSEMBLY.md.
- Keep electronics dry. Initial ambient range: 15–30°C. No condensation on the PCB.

## Revision-K changes retained in L

1. The unqualified slow LTC4365/MOSFET input was replaced by TPS26630RGER with active inrush and overcurrent control. MODE is open for latch-off following the specified overload fault; not every fault type latches.
2. Q2 = IRLB4030PBF; Q3 = BSS138LT1G. Do not fit the old pair of IRL540Ns. Q2 source connects to +24_FUSED, drain to EFUSE_IN.
3. XIAO D1 now measures total current; its old 3.3 V divider was removed.
4. U15 permits relay energization only with POWER_ARM and eFuse PGOOD; R19 holds the request low during reset.
5. Connectors, header spacing, order codes, copper stack and via-in-pad processing are specified.
6. Gerbers, drills, BOM, placement files and design reports are included; see FABRICATION.md.

## First power-up

1. Inspect component orientation and polarity. Check for persistent shorts between +24_IN/GND, +24_PROT/GND, +5_AUX/GND and +3V3/GND, allowing for capacitors charging during measurement. Inspect all five QFNs under magnification.
2. Disconnect glass, TECs, fan and XIAO. Leave JP1 open. Start with a 24 V lab supply limited to 0.25 A. R19/R21 should hold the relay off. Increase to 0.5 A only if bulk charging reaches the supply limit and no persistent fault exists. Check +24_PROT and +5_AUX; 5 V must meet the module specification.
3. Insert XIAO only while unpowered. Program through USB with JP1 open. Use GND as the instrument reference: adapter return is connected to protective earth. Do not connect an earth-referenced scope clip to floating gate/source or shunt nodes; use a differential probe.
4. Program the supplied firmware as described in firmware/README.md. Previous build and host tests passed; calibration and hardware tests remain necessary. With real loads disconnected, verify that reset, I²C faults, disconnected sensors and missing thermostat contacts cannot enable power. Fit the intended NC contacts at J6/J7/J8. Test bridges are allowed only for supervised resistor tests and must then be removed.
5. Test the heater output first with approximately 10 Ω / 100 W on a nonflammable fixture, current limited to 0.5 A. Then test one TEC channel with approximately 4.7 Ω / 50 W. Verify current adjustment, EN-off behavior, CC/CV transition and overshoot before increasing current.
6. Calibrate total input and heater current at two known loads against external meters. Test eFuse overload using an electronic load on +24_PROT only after preceding checks. Confirm latch-off/reset. Do not short the supply with a loose wire.
7. Then connect the Mean Well, heatsink/fan and one real TEC. Mount its hot-side cooling before applying power; verify polarity at low current. A four-channel test requires a stable qualified single channel and working hardware shutdown paths, plus suitable later firmware.
8. Connect glass last. Measure full-busbar resistance, preferably four-wire. Start at 10 W with short pulses, recording all glass temperatures and bond integrity. Increase gradually to 20/40 W and only then a permitted boost after qualification. Initial glass software stop: 40°C; independent NC overtemperature protection approximately 50°C. This does not qualify coating, adhesive or glass for 50°C; any lower material limit takes precedence.

## Physical work still outstanding

Measure eFuse hotplug, loop stability/ripple, measurement accuracy, reset/fault response, component and wiring temperatures, and defogging speed. Record actual results in `commissioning-results.csv`. Full power and unattended operation require successful qualification first.
