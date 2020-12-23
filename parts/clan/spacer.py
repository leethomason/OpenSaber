import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole

mat = init_material(sys.argv[1])

DZ_PLATE = 36.0
W_PLATE = 19.0
DZ_PORT, DZ_BOLT, DZ_SWITCH = 8, 19, 29.3
H_PLATE = 7.575
D_SWITCH = 8.2

depth = -2.0

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

travel(g, mat, y=DZ_PORT)
g.move(z=0)
hole(g, mat, depth, d=11.0, fill=False)

travel(g, mat, y=DZ_BOLT)
g.move(z=0)
hole(g, mat, -2.0, d=8.0, fill=False)

travel(g, mat, y=DZ_SWITCH)
g.move(z=0)
hole(g, mat, depth, d=D_SWITCH, fill=False)

travel(g, mat, x=0, y=0)
g.move(z=0)
rectangleTool(g, mat, depth, W_PLATE, DZ_PLATE, W_PLATE/2, "bottom", "outer", fill=False, adjust_trim=True)
