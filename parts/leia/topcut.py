from utility import *
from material import init_material
from capsule import capsule
from mecode import G

# inner line to inner line: 47mm
# 2.5mm space
# capsule at center

mat = init_material("np883-aluminum-3.175")
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g)

# at center of switch hole; move to center of capsule.
g.move(y=42.0/2 - 16.2/2)
capsule(g, mat, -10, 42.0, 16.2, None, True, 'y')



