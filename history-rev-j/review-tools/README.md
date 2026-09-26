Run python3 check_kelvin.py with Shapely installed. Geometry snapshot is from Rev J. Regenerate polygons from KiCad after any copper changes. This verifies topology only, not EMC or absolute measurement accuracy.

Run `python review-tools/check_bias.py` for the four BIAS branches. Both checks
use the included Rev J copper geometry snapshot; dependencies: Python and Shapely.
