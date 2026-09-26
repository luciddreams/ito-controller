"""Reproduce Rev I qualification screening; standard-library Python 3 only.
All numerical results are analytical scenarios, NOT prototype measurements.
"""
import json, math
from pathlib import Path

def input_current(v, output_w, eta=.85, aux_w=15.125, pair_r=.252):
    power=output_w/eta+aux_w
    disc=v*v-4*pair_r*power
    if disc < 0: return None
    return 2*power/(v+math.sqrt(disc))

def calculate():
    # Bounds below use the particular datasheet test conditions, not all loads.
    low=.958*(1+1.13e6*.99/(100e3*1.01))-20e-9*1.13e6*1.01
    high=.982*(1+1.13e6*1.01/(100e3*.99))+20e-9*1.13e6*1.01
    current_screen=.056/(.025*.99)
    rows=[]
    for v in [20,21,22,23,24,26,28]:
        available=.85*(6*v-.252*6**2-15.125)
        rows.append({'vin_v':v,'input_a_at_96W':input_current(v,96),
                     'tec_budget_w_at_6A':available,
                     'per_channel_a_at_12V_capped_at_2A':min(2,available/48)})
    inrush=[]
    for gate in [9e-9,11e-9]:
        for ig in [12e-6,30e-6]:
            t=28*gate/ig; i=600e-6*ig/gate
            inrush.append({'gate_f':gate,'gate_charge_a':ig,'duration_s':t,
                           'capacitive_a':i,'initial_pair_power_w':28*i,
                           'linear_pass_energy_j':.5*28*i*t})
    thermal=[]
    for ambient in [25,40,60]:
        for sink in [5,10,15]:
            power=6**2*.126
            thermal.append({'air_c':ambient,'separate_sink_k_w':sink,
                            'junction_scenario_c':ambient+power*(1.1+.5+sink)})
    # Frequency 450-550kHz is a chosen sensitivity sweep, not a guaranteed range.
    buck=[]
    for vin in [20,24,28]:
        for l in [8e-6,10e-6,12e-6]:
            for f in [450e3,500e3,550e3]:
                ripple=(vin-high)*high/vin/(l*f)
                for cap in [50e-6,81.78e-6,94e-6]:
                    buck.append({'vin_v':vin,'vout_v':high,'l_h':l,'f_hz':f,'c_f':cap,
                                 'delta_i_pp_a':ripple,'peak_a_at_2A':2+ripple/2,
                                 'capacitive_ripple_pp_v':ripple/(8*f*cap)})
    return {'status':'ANALYTICAL SCREENING ONLY; NO HARDWARE QUALIFICATION',
            'cv_screen_v':[low,high],'current_screen_a_at_56mV_1percent_shunt':current_screen,
            'voltage_for_96W_at_6A_scenario':(96/.85+15.125)/6+.252*6,
            'input_budget':rows,'inrush':inrush,'thermal':thermal,'buck_sensitivity':buck,
            'limits':['85% efficiency and 0.126 ohm hot MOSFET resistance are assumptions',
                      'CV screen excludes resistor TCR, leakage, parasitics and dynamic overshoot',
                      '56mV sense specification applies at the datasheet stated test point',
                      '600uF inrush ignores intrinsic gate charge and active auxiliary loads',
                      'No SOA or control-loop stability pass is generated']}

if __name__=='__main__':
    target=Path(__file__).with_name('calculated-results.json')
    target.write_text(json.dumps(calculate(),indent=2)+'\n')
    print(target)
