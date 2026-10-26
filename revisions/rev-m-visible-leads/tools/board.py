"""Place revision M on an enlarged160x128mm board; preserve lower circuitry.
Uses KiCad9 pcbnew. Removes obsolete copper before changing pin assignments.
Power routing is added separately; no fabrication release is implied.
"""
from pathlib import Path
import pcbnew as p,json,sys,re
R=Path(__file__).resolve().parents[1];B=Path(sys.argv[1]);d=json.loads((R/'design.json').read_text());by={c['ref']:c for c in d};paths=json.loads((R/'tools/component-paths.json').read_text())
b=p.LoadBoard(str(B/'ito-controller.kicad_pcb'));b.SetLayerType(p.In2_Cu,p.LT_SIGNAL);mm=p.FromMM
def xy(x,y):return p.VECTOR2I(mm(x),mm(y))
def pos(o):return p.ToMM(o.GetPosition().x),p.ToMM(o.GetPosition().y)
# Current project nets, new nets have persistent KiCad assignments on save.
for c in d:
 for _,_,net,_ in c['pins']:
  if net and not b.FindNet(net):b.Add(p.NETINFO_ITEM(b,net))
oldfp={f.GetReference():f for f in b.GetFootprints()}
channel={c['ref'] for c in d if c['section'][:2] in ['07','08','09','10']}
# Lifted upper circuit region and changed U1 pad field. No orphan QFN traces remain.
for t in list(b.GetTracks()):
 s=t.GetStart();e=t.GetEnd();n=t.GetNetname()
 top=min(p.ToMM(s.y),p.ToMM(e.y))<44
 uf=min(p.ToMM(s.x),p.ToMM(e.x))<35.5 and max(p.ToMM(s.x),p.ToMM(e.x))>21 and min(p.ToMM(s.y),p.ToMM(e.y))<100.5 and max(p.ToMM(s.y),p.ToMM(e.y))>91
 if top or uf or re.fullmatch(r'T[1-4]_.*|EN[1-4]|ICTRL[1-4]',n):b.Delete(t)
 else:t.Move(xy(0,18))
for z in [z for z in b.Zones()]:
 if z.GetIsRuleArea():z.Move(xy(0,18))
 else:b.Delete(z)
for item in list(b.GetDrawings()):
 if item.GetLayer()==p.Edge_Cuts:
  s=item.GetStart();e=item.GetEnd()
  if s.y>=mm(44):s.y+=mm(18)
  if e.y>=mm(44):e.y+=mm(18)
  item.SetStart(s);item.SetEnd(e)
 elif pos(item)[1]<44:b.Delete(item)
 else:item.Move(xy(0,18))
for ref,f in list(oldfp.items()):
 if ref not in channel and ref not in ['H1','H2','FID1','FID2']:f.Move(xy(0,18))
for c in d:
 ref=c['ref'];f=oldfp.get(ref);name=c['foot'].split(':')[-1]
 if f is None or str(f.GetFPID().GetLibItemName())!=name:
  new=p.FootprintLoad(str(R/'ITO_PCB.pretty'),name)
  if not new:raise RuntimeError(name)
  new.SetFPID(p.LIB_ID('ITO_PCB',name));new.SetReference(ref)
  if f:
   new.SetPosition(f.GetPosition());new.SetOrientation(f.GetOrientation());b.Delete(f)
  b.Add(new);f=new;oldfp[ref]=f
 f.SetValue(c['value']);f.SetPath(p.KIID_PATH(paths[ref]['path']+'/'+paths[ref]['uuid']))
 if c.get('dnp'):f.SetDNP(True)
 pn={str(i):n for i,_,n,_ in c['pins']}
 for pad in f.Pads():
  num=pad.GetNumber()
  if num and num not in pn:raise ValueError((ref,num))
  if num and pn[num]:pad.SetNet(b.FindNet(pn[num]))
  else:pad.SetNetCode(0)
 f.Reference().SetTextSize(xy(.8,.8));f.Reference().SetTextThickness(mm(.12));f.Value().SetVisible(False)
# Mechanical limits changed only in y; upper mounting holes and TEC connectors retained.
layout={
'U':(9,27,0),'F':(8,9,0),'L':(32,29,0),'RS':(32,40,180),'J':(33.08,10,180),'TP':(5,58,0),'MON':(20,55,0),
'C1':(27,18,0),'C2':(33,18,0),'C3':(3,25,90),'C4':(5,20,0),'C5':(15,22,0),'C6':(4,32,0),'C7':(27,48,0),'C8':(36,48,0),'C9':(13,51,0),'C10':(11.5,33,90),'C11':(4,36,0),'C12':(5,15,0),'C13':(13,55,0),'C14':(20,59,0),
'R1':(11,39,0),'R2':(5,40,0),'R3':(5,44,0),'R4':(15,11,0),'R5':(15,26,0),'R6':(17,39,0),'R7':(17,42,0),'R8':(13,47,0),'R9':(18,7,0),'R10':(21,37,0),'Q1':(19,17,0),'Q2':(19,32,0)}
for ch in range(1,5):
 bnum=100*ch;mapping={'U':f'U{10+ch}','F':f'F{10+ch}','L':f'L{ch}','RS':f'RS{ch}','J':f'J{10+ch}','TP':f'TP{10+ch}','MON':f'U{20+ch}'}
 mapping.update({f'C{i}':f'C{bnum+i}' for i in range(1,15)});mapping.update({f'R{i}':f'R{bnum+i}' for i in range(1,11)});mapping.update({f'Q{i}':f'Q{bnum+i}' for i in [1,2]})
 for key,(x,y,ang) in layout.items():
  ref=mapping[key];f=oldfp[ref];f.SetOrientationDegrees(ang);f.SetPosition(xy(x+38*(ch-1),y));f.Reference().SetPosition(xy(x+38*(ch-1),y-2.5 if key[0] not in ['Q','L','U'] else y-4.5));f.Reference().SetTextAngle(p.EDA_ANGLE(0,p.DEGREES_T))
oldfp['C6'].SetPosition(xy(23.5,111.2))
# Preserve original self-contained models by pointing at repository shared directory.
for f in b.GetFootprints():
 for model in f.Models():model.m_Filename=model.m_Filename.replace('${KIPRJMOD}/3dmodels/','${KIPRJMOD}/../../3dmodels/')
# Continuous reference plane and outer heat-spreading pours.
for layer in [p.F_Cu,p.In1_Cu,p.B_Cu]:
 z=p.ZONE(b);z.SetLayer(layer);z.SetNet(b.FindNet('GND'));z.SetLocalClearance(mm(.2));z.SetThermalReliefGap(mm(.25));z.SetThermalReliefSpokeWidth(mm(.4));z.SetPadConnection(p.ZONE_CONNECTION_FULL);z.SetMinThickness(mm(.2));z.Outline().NewOutline()
 for x,y in [(1,1),(159,1),(159,127),(1,127)]:z.Outline().Append(int(mm(x)),int(mm(y)))
 b.Add(z)
# Exposed-pad cooling to inner/bottom ground. Small drilled holes; assembler must manage paste loss.
for ref in ['U1','U11','U12','U13','U14']:
 x,y=pos(oldfp[ref])
 for dx in [-.65,.65]:
  for dy in [-.7,.7]:
   v=p.PCB_VIA(b);v.SetPosition(xy(x+dx,y+dy));v.SetWidth(p.F_Cu,mm(.55));v.SetDrill(mm(.25));v.SetViaType(p.VIATYPE_THROUGH);v.SetLayerPair(p.F_Cu,p.B_Cu);v.SetNet(b.FindNet('GND'));b.Add(v)
# Board identity must not imply revision-L qualification.
t=p.PCB_TEXT(b);t.SetText('ITO / TEC - REV M - ENGINEERING PROTOTYPE');t.SetPosition(xy(78,124));t.SetLayer(p.F_SilkS);t.SetTextSize(xy(1.1,1.1));t.SetTextThickness(mm(.18));b.Add(t)
b.GetTitleBlock().SetRevision('M - prototype');b.GetTitleBlock().SetDate('2026-10-11')
p.SaveBoard(str(R/'ito-controller.kicad_pcb'),b)
print('Placed',len(list(b.GetFootprints())),'footprints;',len(list(b.GetTracks())),'retained tracks/vias. Board160x128mm.')
