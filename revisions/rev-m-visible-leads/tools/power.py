"""Explicit power and Kelvin routing before constrained signal autorouting."""
import pcbnew as p
from pathlib import Path
R=Path(__file__).resolve().parents[1];b=p.LoadBoard(str(R/'ito-controller.kicad_pcb'));fs={f.GetReference():f for f in b.GetFootprints()};mm=p.FromMM

def xy(x,y):return p.VECTOR2I(mm(x),mm(y))
def pp(ref,num):
 pad=next(z for z in fs[ref].Pads() if z.GetNumber()==str(num));pt=pad.GetPosition();return (p.ToMM(pt.x),p.ToMM(pt.y))
def route(net,pts,width=.25,layer=p.F_Cu):
 for a,c in zip(pts,pts[1:]):
  if a==c:continue
  t=p.PCB_TRACK(b);t.SetStart(xy(*a));t.SetEnd(xy(*c));t.SetWidth(mm(width));t.SetLayer(layer);t.SetNet(b.FindNet(net));t.SetLocked(True);b.Add(t)
def via(net,x,y,dia=.65,drill=.3):
 v=p.PCB_VIA(b);v.SetPosition(xy(x,y));v.SetWidth(p.F_Cu,mm(dia));v.SetDrill(mm(drill));v.SetViaType(p.VIATYPE_THROUGH);v.SetLayerPair(p.F_Cu,p.B_Cu);v.SetNet(b.FindNet(net));v.SetLocked(True);b.Add(v)
for ch in range(1,5):
 off=(ch-1)*38;n=f'T{ch}_';c=ch*100
 def loc(points):return [(x+off,y) for x,y in points]
 def r(s,points,w=.25,layer=p.F_Cu):route(n+s,loc(points),w,layer)
 def v(s,x,y,dia=.65,drill=.3):via(n+s,x+off,y,dia,drill)
 # High-current paths, 70um outer copper. Signals do not replace these paths.
 r('VIN',[(10.455,9),(11.5,9),(11.5,5),(21.035,5),(21.035,17)],1.5)
 r('VIN',[(21.035,17),(23.5,18),(25.525,18)],1.5)
 r('VIN',[(25.525,18),(25.525,20.5),(31.525,20.5),(31.525,18)],1.5)
 r('SW',[(14.775,19.28),(16,20.505),(16,23.4),(22.5,29.9),(21.035,31.365),(21.035,32)],1.5)
 r('SW',[(21.035,32),(23.45,32),(26.45,29)],1.5)
 r('SW',[(15.95,22),(16,22)],.5)
 r('VREG',[(37.55,29),(38.5,29),(38.5,40),(34.855,40)],1.5)
 r('OUT',[(29.145,40),(24.31,44.5),(24.31,48)],1.5)
 r('OUT',[(24.31,44.5),(33.31,44.5),(33.31,48)],1.5)
 # Output connector receives power through three parallel vias and a wide bottom trace.
 for yy in [45.2,46.2,47.2]:v('OUT',24.31,yy,.8,.4)
 r('OUT',[(24.31,46.2),(38.5,46.2),(38.5,10),(33.08,10)],1.5,p.B_Cu)
 # Low-side power return and local input-cap return to the ground reference plane.
 route('GND',loc([(14.775,34.28),(14.775,36)]),1.5)
 for x,y in [(14.3,36),(15.3,36),(14.8,36.9),(28.475,16.2),(28.475,15.2),(34.475,16.2),(34.475,15.2)]:via('GND',x+off,y,.8,.4)
 route('GND',loc([(14.3,36),(15.3,36),(14.8,36.9),(14.3,36)]),.8)
 for x in [28.475,34.475]:route('GND',loc([(x,18),(x,15.2)]),.8)
 # Kelvin pickups begin at the shunt pads, not at the inductor/capacitor routing.
 for s,px,ry,padnum in [('VREG',34.855,38.1,1),('OUT',29.145,41.9,2)]:
  r(s,[(px,40),(px,ry)],.25);v(s,px,ry)
 # Use inner routing layer for the sense pair; only this branch feeds sense resistors.
 r('VREG',[(34.855,38.1),(32.8,36.2),(16.0875,36.2),(16.0875,39)],.25,p.In2_Cu);v('VREG',16.0875,39)
 r('OUT',[(29.145,41.9),(27,43.5),(16.0875,43.5),(16.0875,42)],.25,p.In2_Cu);v('OUT',16.0875,42)
 # Monitor taps share the Kelvin pickup, carrying only amplifier input current.
 r('VREG',[(16.0875,39),(15.1,40),(15.1,55.95),(18.65,55.95)],.25,p.B_Cu);v('VREG',18.65,55.95)
 r('OUT',[(16.0875,42),(17,42.9),(17,53),(21.35,53),(21.35,55.95)],.25,p.B_Cu);v('OUT',21.35,55.95)
 # At input fuse, 3 vias avoid relying on a single via for channel feed.
 for y in [8,9,10]:via('+24_LOAD',5.545+off,y,.8,.4)
 route('+24_LOAD',loc([(5.545,8),(5.545,10)]),1.2)
 route('+24_LOAD',loc([(5.545,9),(3,11.545),(3,63)]),1.5,p.B_Cu)
 # Gate-driver supply copper; preserve the close capacitor-to-ground returns.
 r('INTVCC',[(3.525,15),(3.525,20),(3.525,22),(5,23.475),(5.5,24.075),(6.1,24.075)],.35)
 for x,y in [(6.475,15),(6.475,20)]:
  route('GND',loc([(x,y),(x+1.2,y)]),.5);via('GND',x+off+1.2,y)
# Wide switched supply trunk. Existing relay contact remains the source.
route('+24_LOAD',[(3,63),(48,63),(50.5,60.5),(76,60.5),(78.5,63),(117,63)],2.5,p.B_Cu)
route('+24_LOAD',[(28,78),(28,66),(25,63)],2.5,p.B_Cu)
# Explicit eFuse pad fan-out; wide main copper will attach at these via clusters.
route('EFUSE_IN',[(27.1,111.075),(25.7,111.075),(24.45,111.2)],.35)
route('EFUSE_IN',[(27.1,111.725),(25.7,111.725),(25.7,111.075)],.35)
route('EFUSE_IN',[(27.1,114.325),(25.7,114.325)],.35)
for x,y in [(25.7,111.075),(25.7,111.725),(25.7,114.325)]:via('EFUSE_IN',x,y,.65,.3)
route('EFUSE_IN',[(25.7,111.075),(25.7,114.325)],1.2,p.B_Cu)
for y in [111.075,111.725,112.375]:
 route('+24_PROT',[(32.9,y),(34.2,y)],.35);via('+24_PROT',34.2,y,.65,.3)
route('+24_PROT',[(34.2,111.075),(34.2,112.375)],1.2,p.B_Cu)
# Existing tracks are fixed. Routing fills only the missing signal connections.
for t in b.GetTracks():t.SetLocked(True)
b.BuildConnectivity();p.ZONE_FILLER(b).Fill(b.Zones());p.SaveBoard(str(R/'ito-controller.kicad_pcb'),b)
print('Added explicit power / Kelvin copper; zones filled.')
