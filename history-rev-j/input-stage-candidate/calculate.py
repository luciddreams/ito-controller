import json,itertools
from pathlib import Path
r=Path(__file__).resolve().parent
out={'status':'CANDIDATE SCREENING; NOT A QUALIFIED RATING'}
out['current_limit_a']={'nominal_datasheet_point':4.5,'min_including_0p1pct_resistor':4.185/1.001,'max_including_0p1pct_resistor':4.815/.999,'warning':'Transient short-circuit current can exceed the regulated limit.'}
out['ramp']={'nominal_ms_28v':20.8e3*28*100e-9*1000,'nominal_capacitive_a_600u':600e-6/(20.8e3*100e-9),'min_ms_28v':28*95e-9/(26*2.225e-6)*1000,'max_ms_28v':28*105e-9/(23.5*1.775e-6)*1000,'max_capacitive_a_600u':600e-6*26*2.225e-6/95e-9,'energy_j_28v_600u':.5*600e-6*28**2}
# Static threshold corners; resistor tolerance and signed input leakage included.
out['thresholds_v']={}
for name,rt in [('uv_rising',156e3),('ov_rising',223e3),('pg_rising',150e3)]:
 vals=[v*(1+top/bot)+leak*top for v,top,bot,leak in itertools.product([1.176,1.224],[rt*.999,rt*1.001],[10e3*.999,10e3*1.001],[-150e-9,150e-9])]
 out['thresholds_v'][name]={'nominal':1.2*(1+rt/10e3),'low':min(vals),'high':max(vals)}
out['thermal_scenario']={'reverse_fet_ohm_hot_assumption':.126,'efuse_ohm_hot_assumption':.07,'at_4a_reverse_fet_w':16*.126,'at_4a_efuse_w':16*.07,'not_guaranteed':True}
out['output_budget_at_4a']=[{'vin_v':v,'total_bus_w_after_assumed_series_losses':v*4-16*(.126+.07),'tec_w_after_15p125w_aux_at_85pct_efficiency':.85*(v*4-16*(.126+.07)-15.125)} for v in [20,24,28]]
(r/'calculated-corners.json').write_text(json.dumps(out,indent=2))
print(json.dumps(out,indent=2))
