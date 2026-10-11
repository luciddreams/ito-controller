"""Compare schematic export, board and design data. Run under KiCad's Python."""
from pathlib import Path
import json,re,sys,xml.etree.ElementTree as E,pcbnew as p
R=Path(__file__).resolve().parents[1];d=json.loads((R/'design.json').read_text());b=p.LoadBoard(str(R/'ito-controller.kicad_pcb'));x=E.parse(sys.argv[1]);refs={c['ref'] for c in d}
expected={(c['ref'],n):net for c in d for n,_,net,_ in c['pins'] if net};xmlnets={}
for net in x.findall('.//nets/net'):
 for node in net.findall('node'):xmlnets[(node.attrib['ref'],node.attrib['pin'])]=net.attrib['name']
boardnets={(f.GetReference(),pad.GetNumber()):pad.GetNetname() for f in b.GetFootprints() for pad in f.Pads() if pad.GetNumber() and f.GetReference() in refs}
issues=[]
for k,v in expected.items():
 for label,nets in [('schematic',xmlnets),('board',boardnets)]:
  if nets.get(k)!=v:issues.append([label,k,v,nets.get(k)])
cs={e.attrib['ref']:e.findtext('value') for e in x.findall('.//components/comp')};fs={f.GetReference():f for f in b.GetFootprints()}
for c in d:
 ref=c['ref'];f=fs.get(ref)
 if cs.get(ref)!=c['value']:issues.append(['schematic-value',ref,c['value'],cs.get(ref)])
 if not f:issues.append(['missing-board-component',ref]);continue
 if f.GetValue()!=c['value']:issues.append(['board-value',ref,c['value'],f.GetValue()])
 if str(f.GetFPID().GetLibItemName())!=c['foot'].split(':')[-1]:issues.append(['footprint',ref])
 if f.IsDNP()!=bool(c.get('dnp')):issues.append(['dnp',ref])
 if re.search(r'QFN|DFN|WSON|BGA|LGA',str(f.GetFPID().GetLibItemName()),re.I):issues.append(['hidden-lead-package',ref])
nums={f.GetReference():sorted({pad.GetNumber() for pad in f.Pads() if pad.GetNumber()}) for f in b.GetFootprints()}
for c in d:
 if nums[c['ref']]!=sorted({n for n,_,_,_ in c['pins']}):issues.append(['pad-number-set',c['ref'],nums[c['ref']]])
result={'source_components':len(d),'schematic_components':len(cs),'board_components_in_design':len(refs & set(fs)),'mechanical_and_fiducials':sorted(set(fs)-refs),'connected_pin_assignments_checked':len(expected),'issue_count':len(issues),'issues':issues}
(R/'review/netlist-audit.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2));sys.exit(bool(issues))
