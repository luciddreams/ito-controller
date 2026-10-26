"""Update native schematics, preserving existing component UUIDs."""
from pathlib import Path
import json,sys,uuid,math,copy
from sexpr import *
ROOT=Path(__file__).resolve().parents[1];BASE=Path(sys.argv[1]);d=json.loads((ROOT/'design.json').read_text());by={c['ref']:c for c in d}
def uid(s):return str(uuid.uuid5(uuid.NAMESPACE_URL,'ito-rev-m/'+s))
def q(s):return json.dumps(s)
def node(s):return parse(s)
root=parse((BASE/'ito-controller.kicad_sch').read_text());rootid=get(root,'uuid')[1]
paths={}
for sh in allof(root,'sheet'):
 fn=prop(sh,'Sheetfile')[2];paths[fn]='/'+rootid+'/'+get(sh,'uuid')[1]
old={};oldlib={};pages={}
for f in BASE.glob('sheet_*.kicad_sch'):
 a=parse(f.read_text());pages[f.name]=a
 for s in allof(a,'symbol'):
  ref=prop(s,'Reference')[2]
  if ref in by:old[ref]=s
 for s in allof(get(a,'lib_symbols'),'symbol'):oldlib[s[1]]=s
# Functional pin groups on new controllers. Repeated ground pins are visible, individually numbered.
def geom(c):
 pp=c['pins'];ref=c['ref'];n=len(pp)
 if ref=='U1':left=[1,2,6,3,4,5,7,8,13,12,10];right=[20,19,18,17,16,15,14,11,9,21]
 elif ref in ['U11','U12','U13','U14']:left=[3,4,8,6,5,9,14,15,2,7,16,21];right=[1,19,17,18,20,11,12,10,13]
 elif ref.startswith('Q') and n==3:left=['1'];right=['2','3']
 else:left=[p[0] for p in pp[:math.ceil(n/2)]];right=[p[0] for p in pp[math.ceil(n/2):]]
 left=list(map(str,left));right=list(map(str,right));g={};pitch=5.08;h=max(len(left),len(right))*pitch+5.08;w=35.56 if n>5 else (20.32 if n>2 else 12.7)
 for nums,side in [(left,-1),(right,1)]:
  for i,num in enumerate(nums):g[num]=(side*(w/2+5.08),h/2-5.08-i*pitch,0 if side==-1 else 180)
 return w,h,g
symbols={};smeta={}
def symbol_def(c):
 ref=c['ref'];w,h,g=geom(c);bits=[f'(symbol "ITO:{ref}" (pin_names (offset 1.016)) (in_bom yes) (on_board yes)',f'(property "Reference" {q(ref)} (at 0 {h/2+3.81} 0) (effects (font (size 1.27 1.27))))',f'(property "Value" {q(c["value"])} (at 0 {-h/2-3.81} 0) (effects (font (size 1.016 1.016))))',f'(symbol "{ref}_0_1" (rectangle (start {-w/2} {h/2}) (end {w/2} {-h/2}) (stroke (width .254) (type default)) (fill (type background))))',f'(symbol "{ref}_1_1"']
 for num,name,net,typ in c['pins']:
  x,y,a=g[num];bits.append(f'(pin {typ} line (at {x} {y} {a}) (length 5.08) (name {q(name)} (effects (font (size 1.016 1.016)))) (number {q(num)} (effects (font (size 1.016 1.016)))))')
 bits.append('))');sd=node('\n'.join(bits))
 if len(c['pins'])==2 and ref[0] in ['R','C','L','F']:
  graph=allof(sd,'symbol')[0];graph[:]=[graph[0],graph[1]]
  def ln(x1,y1,x2,y2):graph.append(node(f'(polyline (pts (xy {x1} {y1}) (xy {x2} {y2})) (stroke (width .254) (type default)) (fill (type none)))'))
  if ref.startswith('C'):
   ln(-6.35,0,-.635,0);ln(.635,0,6.35,0);ln(-.635,-2.54,-.635,2.54);ln(.635,-2.54,.635,2.54)
  elif ref.startswith('L'):
   ln(-6.35,0,-5.08,0);ln(5.08,0,6.35,0)
   for x in [-5.08,-2.54,0,2.54]:graph.append(node(f'(arc (start {x} 0) (mid {x+1.27} 1.27) (end {x+2.54} 0) (stroke (width .254) (type default)) (fill (type none)))'))
  else:
   ln(-6.35,0,-3.81,0);ln(3.81,0,6.35,0);graph.append(node('(rectangle (start -3.81 1.27) (end 3.81 -1.27) (stroke (width .254) (type default)) (fill (type none)))'))
 return sd
def instance(c,x,y,path):
 ref=c['ref'];w,h,g=geom(c);sid=get(old[ref],'uuid')[1] if ref in old else uid('component/'+ref);smeta[ref]=dict(uuid=sid,path=path)
 bits=[f'(symbol (lib_id "ITO:{ref}") (at {x} {y} 0) (unit 1) (in_bom yes) (on_board yes) (dnp {"yes" if c.get("dnp") else "no"}) (uuid {sid})',f'(property "Reference" {q(ref)} (at {x} {y-h/2-2.54} 0) (effects (font (size 1.27 1.27))))',f'(property "Value" {q(c["value"])} (at {x} {y+h/2+2.54} 0) (effects (font (size 1.016 1.016))))',f'(property "Footprint" {q(c["foot"])} (at {x} {y} 0) (effects (font (size 1 1)) hide))',f'(property "MPN" {q(c.get("mpn",""))} (at {x} {y} 0) (effects (font (size 1 1)) hide))',f'(instances (project "ito-controller" (path {q(path)} (reference {q(ref)}) (unit 1)))))']
 out=[node('\n'.join(bits))]
 for num,name,net,typ in c['pins']:
  px,py,a=g[num];px+=x;py=y-py;tag=f'{ref}/{num}'
  if net is None:out.append(node(f'(no_connect (at {px} {py}) (uuid {uid(tag+"/nc")}))'));continue
  dx=-7.62 if a==0 else 7.62;ex=px+dx
  out.append(node(f'(wire (pts (xy {px} {py}) (xy {ex} {py})) (stroke (width 0) (type default)) (uuid {uid(tag+"/wire")}))'))
  # Global labels: face toward component, text extends outward.
  ang=0 if a==0 else 180
  out.append(node(f'(global_label {q(net)} (shape bidirectional) (at {ex} {py} {ang}) (effects (font (size 1.016 1.016)) (justify {"left" if ang==0 else "right"})) (uuid {uid(tag+"/label")}))'))
 return out
for num in [2,7,8,9,10]:
 fn=f'sheet_{num:02}.kicad_sch';a=pages[fn];cs=[c for c in d if c['section'].startswith(f'{num:02}')]
 # A2 allows readable labels and component identities without crowding.
 title='Input protection - visible leads' if num==2 else f'TEC {num-6} - LT3741-1 CC/CV power stage'
 na=node(f'(kicad_sch (version 20231120) (generator "eeschema") (uuid {get(a,"uuid")[1]}) (paper "A2") (title_block (title {q(title)}) (date "2026-10-11") (rev "M - ENGINEERING PROTOTYPE")) (lib_symbols))')
 lib=get(na,'lib_symbols')
 major=next(c for c in cs if c['ref']==('U1' if num==2 else f'U{num+4}'))
 ordered=[major]+[c for c in cs if c!=major]
 positions={major['ref']:(76.2,88.9)}
 # Controller at left; component blocks grouped into columns for supply, power, control, monitoring.
 if num!=2:
  ch=num-6;b=ch*100
  groups=[[f'F{10+ch}',f'C{b+1}',f'C{b+2}',f'C{b+3}',f'C{b+4}',f'C{b+12}',f'C{b+11}'],[f'Q{b+1}',f'Q{b+2}',f'R{b+4}',f'R{b+5}',f'R{b+9}',f'R{b+10}',f'C{b+5}'],[f'L{ch}',f'RS{ch}',f'C{b+7}',f'C{b+8}',f'J{10+ch}',f'R{b+2}',f'R{b+3}'],[f'R{b+6}',f'R{b+7}',f'C{b+10}',f'R{b+8}',f'C{b+13}',f'C{b+9}']]
  positions[f'U{20+ch}']=(76.2,205.74);positions[f'C{b+14}']=(76.2,261.62);positions[f'TP{10+ch}']=(76.2,307.34);positions[f'R{b+1}']=(177.8,368.3);positions[f'C{b+6}']=(279.4,368.3)
  for col,refs in enumerate(groups):
   for row,ref in enumerate(refs):positions[ref]=(177.8+col*101.6,50.8+row*45.72)
 else:
  rest=[c for c in cs if c!=major]
  for i,c in enumerate(rest):positions[c['ref']]=(177.8+(i//8)*127,50.8+(i%8)*40.64)
 for i,heading in enumerate(['CONTROLLER / MONITOR','SUPPLY / DRIVER BYPASS','SWITCHES / GATE DRIVE','OUTPUT / VOLTAGE LIMIT','SENSE / COMPENSATION'] if num!=2 else ['EFUSE CONTROL','INPUT COMPONENTS','PROTECTION / RESET','INTERLOCK / INPUT']):
  hx=76.2 if i==0 else (177.8+(i-1)*(101.6 if num!=2 else 127))
  na.append(node(f'(text {q(heading)} (at {hx} 20.32 0) (effects (font (size 1.524 1.524) (thickness .254))) (uuid {uid(fn+heading)}))'))
 for c in ordered:
  sd=symbol_def(c);lib.append(sd);symbols[sd[1]]=sd
  na.extend(instance(c,*positions[c['ref']],paths[fn]))
 # Human-readable block notes, power path summary, and explicit validation status.
 notes=['24 V nominal; actual total input current <= 4 A.', 'Control: DAC -> divider -> OPA2333 -> CTRL1. EN remains hardware-interlocked.', 'Power: VIN -> upper FET -> inductor -> 25m shunt -> output capacitors -> TEC.', 'TP monitor = 0.5 V/A average inductor current. Prototype qualification required.'] if num!=2 else ['24 V input. MODE open: latch-off. Reset through J9.', 'TPS26631 permits nominal 25.5 ms at 2x IOL after PGOOD.', 'This changes overload transients compared with revision L. See design review.']
 for i,t in enumerate(notes):na.append(node(f'(text {q(t)} (at 15.24 {395+i*4.5} 0) (effects (font (size 1.27 1.27)) (justify left)) (uuid {uid(fn+str(i))}))'))
 # Bottom page margin must accommodate notes; A2 height420 titlebottom notes overlap title at395: place top description later.
 (ROOT/fn).write_text(dump(na)+'\n')
# Update unchanged sheet values in both embedded symbols and placed instances.
for fn,a in pages.items():
 if int(fn[6:8]) in [2,7,8,9,10]:continue
 for s in allof(a,'symbol'):
  pr=prop(s,'Reference');ref=pr[2] if pr else ''
  if ref in by:
   prop(s,'Value')[2]=by[ref]['value'];smeta[ref]=dict(uuid=get(s,'uuid')[1],path=paths[fn])
 for s in allof(get(a,'lib_symbols'),'symbol'):
  ref=s[1].split(':')[-1]
  if ref in by and prop(s,'Value'):prop(s,'Value')[2]=by[ref]['value']
 get(get(a,'title_block'),'rev')[1]='M - visible leads';get(get(a,'title_block'),'date')[1]='2026-10-11'
 (ROOT/fn).write_text(dump(a)+'\n')
# Root titles; remove the redundant +24_PROT flag now U1 OUT is explicitly power_out.
for s in list(allof(root,'symbol')):
 if prop(s,'Reference') and prop(s,'Reference')[2]=='#FLG3':root.remove(s)
for lab in list(allof(root,'global_label')):
 if lab[1]=='+24_PROT':root.remove(lab)
get(get(root,'title_block'),'rev')[1]='M - ENGINEERING PROTOTYPE';get(get(root,'title_block'),'date')[1]='2026-10-11'
# Legitimate derived power sources after fuse / reverse-blocking FET, explicitly labelled.
flag=next(s for s in allof(root,'symbol') if prop(s,'Reference')[2]=='#FLG2')
for i,net in enumerate(['+24_FUSED','EFUSE_IN','T1_VIN','T2_VIN','T3_VIN','T4_VIN']):
 x=30.48+i*63.5;y=261.62;r=f'#FLG{20+i}';s=copy.deepcopy(flag);get(s,'at')[1:3]=[Atom(str(x)),Atom(str(y))];get(s,'uuid')[1]=Atom(uid(r));prop(s,'Reference')[2]=r
 for p in allof(s,'property'):get(p,'at')[1:3]=[Atom(str(x)),Atom(str(y+3.81))]
 inst=get(get(get(s,'instances'),'project'),'path');get(inst,'reference')[1]=r
 root.append(s);root.append(node(f'(global_label {q(net)} (shape bidirectional) (at {x} {y} 0) (effects (font (size 1 1)) (justify left)) (uuid {uid(r+"/label")}))'))
(ROOT/'ito-controller.kicad_sch').write_text(dump(root)+'\n')
# Synchronize project-local symbol library with changed sheets.
library=parse((BASE/'ITO.kicad_sym').read_text())
for s in list(allof(library,'symbol')):
 key='ITO:'+s[1].split(':')[-1]
 if key in symbols:library.remove(s)
for key,s in symbols.items():s=copy.deepcopy(s);s[1]=key.split(':')[-1];library.append(s)
(ROOT/'ITO.kicad_sym').write_text(dump(library)+'\n');(ROOT/'tools/component-paths.json').write_text(json.dumps(smeta,indent=2)+'\n')
print('Updated 5 circuit sheets; retained existing UUIDs; paths',len(smeta))
