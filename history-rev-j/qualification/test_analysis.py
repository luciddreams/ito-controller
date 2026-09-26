"""Synthetic software checks only; these are NOT physical test results."""
import unittest
from calculate import calculate,input_current
from analyze_capture import analyze
class Checks(unittest.TestCase):
    def test_power_balance(self):
        for v in [20,24,28]:
            i=input_current(v,96)
            self.assertAlmostEqual(v*i-.252*i*i,96/.85+15.125)
    def test_inrush_corner_energy(self):
        for row in calculate()['inrush']:
            self.assertAlmostEqual(row['linear_pass_energy_j'],.5*600e-6*28**2)
    def test_nonuniform_linear_waveform(self):
        c={'origin':'SYNTHETIC','samples':[{'t_s':t,'vds_v':28*(1-t/.02),'id_a':1,'vgs_v':8} for t in [0,.003,.011,.02]]}
        self.assertAlmostEqual(analyze(c)['signed_energy_j'],.28)
        self.assertIn('NOT ASSESSED',analyze(c)['SOA_status'])
    def test_bad_capture(self):
        for times in [[0,0],[1,0]]:
            with self.assertRaises(ValueError):analyze({'samples':[{'t_s':t,'vds_v':1,'id_a':1,'vgs_v':1} for t in times]})
if __name__=='__main__':unittest.main()
