import json
from pathlib import Path
from shapely.geometry import Polygon
from shapely.ops import unary_union
r=Path(__file__).resolve().parent.parent;geo=json.loads((r/'review-tools/copper-geometry.json').read_text());out=[]
jobs=[(f'T{k}_{n}',f'U{k+10}:{pin}',f'RS{k}:{sp}') for k in range(1,5) for n,pin,sp in [('VREG',25,1),('OUT',26,2)]]+[('HEATER_FUSED','U3:10','RSH1:1'),('ITO_PLUS','U3:9','RSH1:2'),('ITO_PLUS','U3:8','RSH1:2')]
for net,source,target in jobs:
 obs=[v for v in geo if v['net']==net];pads=[v for v in obs if v.get('id')==target];cut=unary_union([Polygon(poly) for v in pads for poly in v['poly']]).buffer(.003)
 shapes=[unary_union([Polygon(poly) for poly in v['poly']]).difference(cut) if v['l']==0 else unary_union([Polygon(poly) for poly in v['poly']]) for v in obs]
 starts=[i for i,v in enumerate(obs) if v.get('id')==source];seen=set(starts);queue=list(starts)
 while queue:
  i=queue.pop()
  for j in range(len(obs)):
   if j in seen or shapes[i].is_empty or shapes[j].is_empty:continue
   # Via polygons are emitted on each layer; physical via joins its own coincident shapes.
   same=obs[i]['l']==obs[j]['l']
   via=(obs[i].get('kind')=='via' and obs[j].get('kind')=='via' and obs[i].get('uid')==obs[j].get('uid'))
   if (same or via) and shapes[i].distance(shapes[j])<.002:seen.add(j);queue.append(j)
 contacts=sorted(set(obs[i].get('id') for i in seen if obs[i].get('id') and obs[i].get('id')!=source));out.append({'net':net,'sense_pin':source,'target_shunt_pad':target,'other_pads_reachable_with_shunt_pad_removed':contacts,'isolated_until_shunt':not contacts,'sense_component_items':len(seen)})
(r/'kelvin-isolation-review.json').write_text(json.dumps(out,indent=2));print(json.dumps(out,indent=2));assert all(v['isolated_until_shunt'] for v in out)
