"""Build revision M component data and new leaded footprints from revision L.
Run with Python 3; input is an immutable revision-L project directory.
"""
import json,sys,copy,csv
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
BASE=Path(sys.argv[1])
d=json.loads((BASE/'design.json').read_text()); by={c['ref']:c for c in d}
def change(ref,value,mpn,foot=None,note='',spec=None):
 c=by[ref];c.update(value=value,mpn=mpn,note=note,status='Revision M engineering prototype; qualification pending')
 if foot:c['foot']='ITO_PCB:'+foot;c['source_footprint']='ITO_PCB:'+foot
 c['specification']=spec if spec is not None else value
 return c
def pins(items):return [[str(n),name,net,typ] for n,name,net,typ in items]
def add(ref,value,mpn,foot,pp,section,manufacturer='',note=''):
 c=dict(ref=ref,value=value,mpn=mpn,foot='ITO_PCB:'+foot,pins=pins(pp),section=section,manufacturer=manufacturer,note=note,status='Revision M engineering prototype; qualification pending');d.append(c);by[ref]=c;return c
def passive(ref,val,mpn,n1,n2,section,cap=False,foot=None,note=''):
 return add(ref,val,mpn,foot or ('C_0805_2012Metric' if cap else 'R_0805_2012Metric'),[(1,'1',n1,'passive'),(2,'2',n2,'passive')],section,'Murata' if cap else 'Yageo',note)
u=change('U1','TPS26631PWPR','TPS26631PWPR','TI_PWP0020T_EP2.96','MODE open = latch-off. 4.5 A nominal sustained limit. TPS26631 permits 2x current pulse after PGOOD for nominal 25.5 ms; not fault-identical to TPS26630.')
u['pins']=pins([(1,'IN','EFUSE_IN','power_in'),(2,'IN','EFUSE_IN','passive'),(3,'B_GATE','PROT_GATE','output'),(4,'DRV','PROT_DRV','output'),(5,'IN_SYS','+24_FUSED','power_in'),(6,'IN','EFUSE_IN','passive'),(7,'UVLO','UV_DIV','input'),(8,'OVP','OV_DIV','input'),(9,'GND','GND','power_in'),(10,'dVdT','PROT_DVDT','passive'),(11,'ILIM','PROT_ILIM','passive'),(12,'MODE',None,'input'),(13,'SHDN','PROT_SHDN','input'),(14,'IMON','INPUT_IMON','output'),(15,'FLT','PROT_FAULT_N','open_collector'),(16,'PGTH','PROT_PGTH','input'),(17,'PGOOD','PROT_PGOOD','open_collector'),(18,'OUT','+24_PROT','power_out'),(19,'OUT','+24_PROT','passive'),(20,'OUT','+24_PROT','passive'),(21,'EP_GND','GND','power_in')])
u['datasheet']='https://www.ti.com/lit/ds/symlink/tps2663.pdf'
# Use conservative full-scale DAC setting. Nominal 1.987 A; calibrate and cap actual current at 2 A.
for ref in ['R42','R44','R46','R48']:
 change(ref,'4k02 0.1%','RT0805BRD074K02L',note='DAC upper divider. 2.048 V * 10k/(10k+4.02k) = 1.4608 V. LT3741 nominal full-scale ~1.987 A using 51 mV reference. Firmware calibration required.')
for ch in range(1,5):
 pre=f'T{ch}_';sec=by[f'U{10+ch}']['section'];b=ch*100
 c=change(f'U{10+ch}','LT3741EFE-1#PBF','LT3741EFE-1#PBF','LT_FE20_EP2.74x3.86','Non-sinking controller. 300 kHz, external N-channel MOSFETs. Sense resistor before output capacitors; differential Kelvin filter. Thermal EP solder required.')
 names=[('VCC_INT',pre+'INTVCC','power_out'),('GND','GND','power_in'),('VIN',pre+'VIN','power_in'),('EN_UVLO',f'EN{ch}','input'),('VREF',pre+'VREF','power_out'),('CTRL2',pre+'VREF','input'),('GND','GND','power_in'),('CTRL1',f'ICTRL{ch}','input'),('SS',pre+'SS','passive'),('FB',pre+'FB','input'),('SENSE+',pre+'SENSE_P','input'),('SENSE-',pre+'SENSE_N','input'),('VC',pre+'VC','passive'),('RT',pre+'RT','passive'),('SYNC','GND','input'),('GND','GND','power_in'),('HG',pre+'HG','output'),('SW',pre+'SW','input'),('CBOOT',pre+'BST','passive'),('LG',pre+'LG','output'),('EP_GND','GND','power_in')]
 c['pins']=pins([(i+1,*v) for i,v in enumerate(names)]);c['datasheet']='https://www.analog.com/media/en/technical-documentation/data-sheets/37411fg.pdf'
 change(f'R{b+1}','143k 1%','RC0805FR-07143KL',note='LT3741 nominal 300 kHz timing resistor.')
 change(f'R{b+2}','88k7 0.1%','RT0805BRD0788K7L',note='Voltage ceiling upper divider; 11.9427 V nominal before FB bias error.')
 change(f'R{b+3}','10k 0.1%','RT0805BRD0710KL',note='Voltage ceiling lower divider.')
 change(f'C{b+4}','22u / 25V X7R','GRM32ER71E226KE15L','C_1210_3225Metric',note='VCC_INT bypass, parallel C112; verify effective combined C >=16uF at 5V.')
 change(f'C{b+6}','100n / 50V X7R','GRM21BR71H104KA01L',note='Approx.13.3 ms current soft-start to 1.46 V.')
 for z in [7,8]:by[f'C{b+z}']['pins'][0][2]=pre+'OUT';by[f'C{b+z}']['note']='Output capacitance AFTER current shunt for average-inductor-current control.'
 c=change(f'L{ch}','33uH SRP1265A-330M','SRP1265A-330M','Bourns_SRP1265A',note='33uH +/-20%; DCR max58mOhm; 8A Irms and11A Isat at25C. Verify hot inductance and temperature rise.');c['manufacturer']='Bourns';c['datasheet']='https://www.bourns.com/docs/product-datasheets/srp1265a.pdf'
 # C109 is a deliberately unpopulated HF compensation option, not legacy feedforward.
 c=change(f'C{b+9}','DNP 100p C0G','GRM21A5C1H101JA01D',note='Optional VC high-frequency pole; DNP initially. Fit only after loop-response measurement.');c['dnp']=True;c['pins'][0][2]=pre+'VC';c['pins'][1][2]='GND'
 for idx,gate,drain,source in [(1,'GH','VIN','SW'),(2,'GL','SW',None)]:
  q=add(f'Q{b+idx}','IRLR024NTRPBF','IRLR024NTRPBF','Infineon_DPAK_TO252_3',[(1,'G',pre+gate,'input'),(2,'D',pre+drain,'passive'),(3,'S',pre+source if source else 'GND','passive')],sec,'Infineon','55V logic N-MOSFET. VGS(th) max2V is at the LT3741 recommended boundary; cold-start/recovery qualification required.');q['datasheet']='https://www.infineon.com/assets/row/public/documents/24/49/infineon-irlr024n-datasheet-en.pdf'
 passive(f'R{b+4}','1R0 1%','RC0805FR-071RL',pre+'HG',pre+'GH',sec,note='High-side gate damping; shortest loop to HG and SW.')
 passive(f'R{b+5}','1R0 1%','RC0805FR-071RL',pre+'LG',pre+'GL',sec,note='Low-side gate damping.')
 passive(f'R{b+6}','10R 0.1%','RT0805BRD0710RL',pre+'VREG',pre+'SENSE_P',sec,note='Kelvin pickup at RS high pad; no shared power trace.')
 passive(f'R{b+7}','10R 0.1%','RT0805BRD0710RL',pre+'OUT',pre+'SENSE_N',sec,note='Kelvin pickup at RS low pad; no shared power trace.')
 passive(f'C{b+10}','33n / 50V X7R','GRM21BR71H333KA01L',pre+'SENSE_P',pre+'SENSE_N',sec,True,note='Differential current-sense filter, adjacent controller.')
 passive(f'C{b+11}','1u / 50V X7R','GRM21BR71H105KA12L',pre+'VREF','GND',sec,True,note='VREF local bypass.')
 passive(f'C{b+12}','22u / 25V X7R','GRM32ER71E226KE15L',pre+'INTVCC','GND',sec,True,'C_1210_3225Metric',note='Parallel C104 for internal gate-driver regulator; effective combined C >=16uF at5V.')
 passive(f'R{b+8}','22k1 1%','RC0805FR-0722K1L',pre+'VC',pre+'COMP',sec,note='Initial compensation below worst-corner calculated R; loop measurement required.')
 passive(f'C{b+13}','6n8 / 50V C0G','GRM2165C1H682JA01D',pre+'COMP','GND',sec,True,note='Series RC compensation. Initial values require bench stability verification.')
 add(f'U{20+ch}','INA180A1IDBVR','INA180A1IDBVR','SOT-23-5',[(1,'OUT',pre+'IMON','output'),(2,'GND','GND','power_in'),(3,'IN+',pre+'VREG','input'),(4,'IN-',pre+'OUT','input'),(5,'VS','+3V3','power_in')],sec,'Texas Instruments','Gain20:0.5V/A nominal. Monitor average inductor current; TP only, no automatic MCU protection.')
 passive(f'C{b+14}','100n / 50V X7R','GRM21BR71H104KA01L','+3V3','GND',sec,True,note='INA180 local supply bypass.')
 by[f'TP{10+ch}']['note']='INA180 output,0.5V/A nominal average inductor current. Unfiltered ripple present. TP only; not MCU feedback.'
 # De-energize FET gates when controller unpowered.
 passive(f'R{b+9}','100k 1%','RC0805FR-07100KL',pre+'GH',pre+'SW',sec,note='High-side gate-source bleed.')
 passive(f'R{b+10}','100k 1%','RC0805FR-07100KL',pre+'GL','GND',sec,note='Low-side gate-source bleed.')
by['Q2']['datasheet']='https://www.infineon.com/assets/row/public/documents/24/49/infineon-irlb4030-datasheet-en.pdf'
by['Q2']['note']=by['Q2']['note'].replace('TPS26630','TPS26631')
(ROOT/'design.json').write_text(json.dumps(d,indent=2)+'\n')
# Native KiCad custom footprints; dimensions from manufacturer land patterns.
lib=ROOT/'ITO_PCB.pretty'
def footprint(name,body,pads,ep=None,desc=''):
 out=[f'(footprint "{name}" (version 20241229) (generator "pcbnew") (layer "F.Cu") (descr "{desc}") (attr smd)', '(property "Reference" "REF**" (at 0 -5 0) (layer "F.SilkS") (effects (font (size 0.9 0.9) (thickness 0.15))))',f'(property "Value" "{name}" (at 0 5 0) (layer "F.Fab") (effects (font (size 0.7 0.7) (thickness 0.1))))']
 w,h=body
 for layer,width in [('F.Fab',.1),('F.CrtYd',.05)]:
  ex=.25 if layer=='F.CrtYd' else 0
  x=max(w/2,max(abs(x)+sx/2 for _,x,y,sx,sy in pads)) if ex else w/2
  y=max(h/2,max(abs(y)+sy/2 for _,x,y,sx,sy in pads)) if ex else h/2
  xmin=min(-w/2,min(px-sx/2 for _,px,py,sx,sy in pads)) if ex else -w/2
  xmax=max(w/2,max(px+sx/2 for _,px,py,sx,sy in pads)) if ex else w/2
  out.append(f'(fp_rect (start {xmin-ex} {-y-ex}) (end {xmax+ex} {y+ex}) (stroke (width {width}) (type default)) (fill none) (layer "{layer}"))')
 for n,x,y,sx,sy in pads:
  layers='"F.Cu" "F.Mask"' if ep and n==ep[0] else '"F.Cu" "F.Paste" "F.Mask"'
  out.append(f'(pad "{n}" smd roundrect (at {x} {y}) (size {sx} {sy}) (layers {layers}) (roundrect_rratio 0.1))')
 if ep:
  _,ew,eh=ep
  # four segmented paste apertures, ~64% coverage, no paste over central via cross.
  for x in [-ew/4,ew/4]:
   for y in [-eh/4,eh/4]:out.append(f'(pad "" smd rect (at {x} {y}) (size {ew*.4} {eh*.4}) (layers "F.Paste"))')
 out.append(f'(fp_circle (center {-w/2} {-h/2-.7}) (end {-w/2+.25} {-h/2-.7}) (stroke (width .12) (type default)) (fill solid) (layer "F.SilkS"))')
 out.append(')');(lib/(name+'.kicad_mod')).write_text('\n'.join(out)+'\n')
for name,ew,eh in [('TI_PWP0020T_EP2.96',2.96,2.96),('LT_FE20_EP2.74x3.86',2.74,3.86)]:
 pp=[]
 for i in range(10):pp.extend([(i+1,-2.9,-2.925+i*.65,1.5,.4),(20-i,2.9,-2.925+i*.65,1.5,.4)])
 pp.append((21,0,0,ew,eh));footprint(name,(4.4,6.5),pp,(21,ew,eh),'20 exposed gull-wing leads at0.65mm pitch; thermal pad segmented paste')
# Infineon DPAK land pattern rotated into conventional left lead/right tab orientation.
footprint('Infineon_DPAK_TO252_3',(6.73,6.22),[(1,-4.225,-2.28,3.21,1.6),(3,-4.225,2.28,3.21,1.6),(2,2.035,0,5.5,6.48)],None,'IRLR024N DPAK:1=gate,2=drain tab,3=source; exposed gate/source leads and drain tab')
footprint('Bourns_SRP1265A',(13.5,12.5),[(1,-5.55,0,3.1,5),(2,5.55,0,3.1,5)],None,'Bourns SRP1265A lead-frame terminal; manufacturer land14.2mm overall,8mm gap,5mm height')
# Remove obsolete footprints from this self-contained revision to avoid accidental reuse.
for n in ['QFN-28-1EP_3x6mm_P0.5mm_EP1.7x4.75mm','TI_RGE0024H_4x4_EP2.7','L_Vishay_IHLP-6767']:
 (lib/(n+'.kicad_mod')).unlink(missing_ok=True)
with (ROOT/'BOM-prototype.csv').open('w',newline='') as f:
 w=csv.writer(f);w.writerow(['Reference','Quantity','Value','Description_EN','Manufacturer','MPN','Footprint','Specification','Assembly','Fit'])
 for c in d:w.writerow([c['ref'],1,c['value'],c.get('note',''),c.get('manufacturer',''),c.get('mpn',''),c['foot'],c.get('specification',''),'SMD' if c['ref'][0] not in ['J','K','H'] else 'Check footprint','DNP' if c.get('dnp') else 'FIT'])
print('Components:',len(d),'new',len(d)-len(json.loads((BASE/'design.json').read_text())))
