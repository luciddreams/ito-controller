#!/usr/bin/env python3
"""Two-point fit only; never invents or writes hardware calibration."""
import argparse, math
p=argparse.ArgumentParser(description='Fit y = slope*x + offset from two measured points.')
p.add_argument('kind',choices=['adc','current'])
p.add_argument('x1',type=float);p.add_argument('y1',type=float)
p.add_argument('x2',type=float);p.add_argument('y2',type=float)
a=p.parse_args()
if not all(math.isfinite(v) for v in (a.x1,a.y1,a.x2,a.y2)) or abs(a.x2-a.x1)<1e-6:p.error('Two distinct finite x values required')
s=(a.y2-a.y1)/(a.x2-a.x1);b=a.y1-s*a.x1
if s<=0:p.error('Slope must be positive; check point order and units')
keys=('ADC_VOLTS_PER_COUNT','ADC_OFFSET_V') if a.kind=='adc' else ('INPUT_AMPS_PER_VOLT','INPUT_OFFSET_A')
for k,v in zip(keys,(s,b)):print(f'{k} = {v:.9g}f;')
print('Validate with an independent third point, then enter config.h manually. This is not a test result.')
