import cadquery as cq,json
from pathlib import Path
p=Path(__file__).resolve().parent
def box(x,y,z,c):return cq.Workplane('XY').box(x,y,z).translate(c)
def save(name,parts):
 a=cq.Assembly(name=name)
 for i,(s,col) in enumerate(parts):a.add(s,name=f'part{i}',color=cq.Color(*col))
 a.save(str(p/(name+'.step')))
black=(.12,.13,.15);metal=(.7,.72,.75);amber=(.9,.5,.12);ceramic=(.62,.46,.28)
# Custom approximations: dimensions recorded in 3D-INSPECTIE.md, not manufacturer CAD.
parts=[(box(29,12.7,15.7,(12.5,-3.75,7.85)),black)]
for x,y in [(0,0),(20,0),(25,0),(25,-7.5),(20,-7.5),(0,-7.5)]:parts.append((box(.5,.8,3.5,(x,y,-1.75)),metal))
save('G2RL_1A_E_review',parts)
save('L_Vishay_IHLP-6767',[(box(17.15,17.15,12.45,(0,0,6.325)),(.28,.28,.29)),(box(3.1,11.94,.25,(-7.1,0,.125)),metal),(box(3.1,11.94,.25,(7.1,0,.125)),metal)])
for name in ['Littelfuse_451','Fuse_2410_6125Metric']:
 save(name,[(box(6.1,2.69,2.69,(0,0,1.345)),(.88,.85,.72)),(box(.7,2.75,2.75,(-2.75,0,1.375)),metal),(box(.7,2.75,2.75,(2.75,0,1.375)),metal)])
save('KEMET_KPS_2220_Double',[(box(5.6,5,2,(0,0,1.6)),ceramic),(box(5.6,5,2,(0,0,3.8)),ceramic),(box(.35,4.7,5,(-2.825,0,2.5)),metal),(box(.35,4.7,5,(2.825,0,2.5)),metal)])
# Actual capacitor height reservation, replacing generic KiCad model which was only 12.5 high.
save('CP_Radial_D12.5mm_P5.00mm',[(cq.Workplane('XY').circle(6.25).extrude(22).translate((2.5,0,0)),(.12,.18,.24)),(cq.Workplane('XY').circle(.35).extrude(3.5).translate((0,0,-3.5)),metal),(cq.Workplane('XY').circle(.35).extrude(3.5).translate((5,0,-3.5)),metal)])
# SSW socket family height 8.51, 2.54 pitch; simplified openings.
b=box(2.54,17.78,8.51,(0,-7.62,4.255))
for i in range(7):b=b.cut(box(.85,.85,2,(0,-i*2.54,8.1)))
parts=[(b,black)]
for i in range(7):parts.append((box(.64,.64,3.3,(0,-i*2.54,-1.65)),metal))
save('PinSocket_1x07_P2.54mm_Vertical',parts)
# Installed module envelope referenced to J1 pin 1; mating male spacer 2.54 assumed.
z=8.51+2.54
parts=[(box(17.8,21,1,(7.62,-7.62,z+.5)),amber),(box(9,7.5,3.2,(7.62,0.7,z+2.6)),metal),(box(11,11,2,(7.62,-11,z+2)),amber)]
for x in [0,15.24]:
 parts.append((box(2.54,17.78,2.54,(x,-7.62,9.78)),black))
 for i in range(7):parts.append((box(.64,.64,4,(x,-2.54*i,9.4)),metal))
save('XIAO_nRF52840_ENVELOPE',parts)
