"""Analyze ONE transistor's measured waveform, without declaring SOA compliance.
Usage: python analyze_capture.py capture.json
Input: {"origin":"measured", "device":"Q2", "samples":[
  {"t_s":0.0,"vds_v":28.0,"id_a":0.0,"vgs_v":0.0}, ...]}
Scope channels must be deskewed, calibrated and captured simultaneously.
"""
import json,math,sys

def analyze(capture):
    rows=capture['samples']
    if len(rows)<2: raise ValueError('At least two samples required')
    keys=['t_s','vds_v','id_a','vgs_v']
    for row in rows:
        if any(not isinstance(row.get(k),(int,float)) or not math.isfinite(row[k]) for k in keys):
            raise ValueError('Missing/nonfinite waveform value')
    t=[r['t_s'] for r in rows]
    if any(b<=a for a,b in zip(t,t[1:])):raise ValueError('Time must increase strictly')
    p=[r['vds_v']*r['id_a'] for r in rows]
    energy=sum((b-a)*(x+y)/2 for a,b,x,y in zip(t,t[1:],p,p[1:]))
    return {'device':capture.get('device'),'origin':capture.get('origin','unspecified'),
            'duration_s':t[-1]-t[0],'peak_id_a':max(r['id_a'] for r in rows),
            'peak_abs_vds_v':max(abs(r['vds_v']) for r in rows),
            'vgs_min_v':min(r['vgs_v'] for r in rows),'vgs_max_v':max(r['vgs_v'] for r in rows),
            'peak_power_w':max(p),'signed_energy_j':energy,
            'SOA_status':'NOT ASSESSED: requires pulse trajectory, case temperature, repetition and manufacturer SOA',
            'warning':'Clipping, bandwidth, omitted pulse portions and channel skew are not detected'}
if __name__=='__main__':
    print(json.dumps(analyze(json.load(open(sys.argv[1]))),indent=2))
