#!/usr/bin/env python3
"""Analytical design estimates, never measured qualification results."""
import json,math
from pathlib import Path
R=Path(__file__).resolve().parents[1]
vdac=2.048*4095/4096;ratio=10000/(10000+4020);ctrl=vdac*ratio;rs=.025;L=33e-6;fs=300e3;vout=1.21*(1+88700/10000)
ripple=lambda vi,vo,ind,f:vo*(1-vo/vi)/(ind*f)
r={
 'status':'CALCULATED ONLY; not a bench result',
 'dac_full_scale_v':vdac,'divider_ratio':ratio,'ctrl_full_scale_v':ctrl,
 'current_full_scale_formula_30_A':ctrl/(30*rs),
 'current_full_scale_51mV_typ_A':ctrl/1.5*.051/rs,
 'current_reference_and_shunt_only_bounds_A':[ctrl/1.5*.048/(rs*1.01),ctrl/1.5*.054/(rs*.99)],
 'voltage_ceiling_nominal_V':vout,
 'inductor_ripple_24V_nominal_App':ripple(24,vout,L,fs),
 'inductor_ripple_28V_nominal_App':ripple(28,vout,L,fs),
 'inductor_ripple_28V_Lminus20_fminus10_App':ripple(28,vout,L*.8,fs*.9),
 'sense_resistor_dc_loss_at2A_W':4*rs,
 'inductor_copper_loss_at2A_25C_maxDCR_W':4*.058,
 'gate_driver_LDO_loss_28V_Qg16nC_each_W':(28-5)*32e-9*fs,
 'gate_driver_capacitance_recommendation_uF':.5*32,
 'soft_start_to_full_scale_ms':ctrl*100e-9/11e-6*1e3,
 'compensation_formula_R_ohm':fs*L*1000/(vout*rs),
 'compensation_R_at_Lminus20_fminus10_ohm':fs*.9*L*.8*1000/(vout*rs),
 'compensation_formula_C_F':.002/fs,
 'selected_compensation_R_ohm':22100,'selected_compensation_C_F':6.8e-9,
 'inductor_monitor_gain_V_per_A':20*rs,
 'efuse_IOL_nominal_A':18000/4020,
 'efuse_2x_overload_pulse_nominal_A':2*18000/4020,
 'efuse_pulse_typical_ms':25.5,
 'nominal_power_four_TEC_at12V2A_W':96,
 'input_current_four_TEC_24V_efficiency90pct_A':96/(24*.9),
 'notes':['Current bounds omit DAC error, divider tolerance, temperature and transient behavior.','Switching frequency +/-10% is an analytical assumption, not a datasheet limit at300kHz.','TP11-14 measure inductor current, not instantaneous TEC current after output capacitors.','The actual input must remain <=4A; all four channels at12V/2A exceed that budget.','IRLR024N VGS(th) maximum2V is on, not below, the LT3741 recommended boundary; startup/recovery needs explicit validation.']}
(R/'review/calculations.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps(r,indent=2))
