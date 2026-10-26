"""Preserve schematic no-connect net names; flag mechanical-only footprints."""
import pcbnew as p,xml.etree.ElementTree as E,sys
from pathlib import Path
R=Path(__file__).resolve().parents[1];f=R/'ito-controller.kicad_pcb';b=p.LoadBoard(str(f));xml=E.parse(sys.argv[1]);nets={}
for n in xml.findall('.//nets/net'):
 for d in n.findall('node'):nets[(d.attrib['ref'],d.attrib['pin'])]=n.attrib['name']
for fp in b.GetFootprints():
 if fp.GetReference().startswith(('H','FID')):fp.SetAttributes(fp.GetAttributes()|p.FP_BOARD_ONLY|p.FP_EXCLUDE_FROM_BOM|p.FP_EXCLUDE_FROM_POS_FILES)
 for pad in fp.Pads():
  net=nets.get((fp.GetReference(),pad.GetNumber()))
  if not pad.GetNetname() and net and net.startswith('unconnected-'):
   if not b.FindNet(net):b.Add(p.NETINFO_ITEM(b,net))
   pad.SetNetCode(b.FindNet(net).GetNetCode())
b.BuildConnectivity();p.ZONE_FILLER(b).Fill(b.Zones());p.SaveBoard(str(f),b)
