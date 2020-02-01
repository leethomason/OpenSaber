from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G

# inner line to inner line: 47mm
# 2.5mm space
# capsule at center

mat = init_material("np883-aluminum-3.175")
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)

# origin at bottom center.
g.move(y=22.2)
g.move(z=0)
rectangleTool(g, mat, -10, 18.0, 42.0, 0, "bottom", 'inner')
