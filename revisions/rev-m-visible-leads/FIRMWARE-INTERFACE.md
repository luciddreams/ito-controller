# Revision M firmware interface

GPIO assignments, I²C addresses, input-current measurement, heater INA226, relay/interlock logic and DAC protocol are unchanged from revision L. The root diagnostic firmware writes raw DAC codes and can be used only after the revision-M qualification/calibration described in `README.md` and `review/DESIGN-REVIEW.md`.

Do not reuse revision-L TEC calibration. R42/R44/R46/R48 are now4.02 kΩ and the regulator is LT3741-1. Full DAC code is not a guaranteed2 A command. Keep `MEASUREMENTS_VERIFIED=false` and `TEC_MAX_CODE={0,0,0,0}` until measured.

TP11–TP14 retain nominal0.5 V/A scale but now report average **inductor** current, including ripple/charging transients. They remain external testpoints with no ADC connection to the MCU. Measure current in the TEC lead when establishing the actual load-current limit.

TPS26631 has a2×/25.5 ms nominal overload pulse feature. Software's input-current trip does not prevent that instantaneous hardware behavior. Validate it separately. Firmware still enables at most one TEC channel during supervised diagnostic pulses and does not provide unattended operation.
