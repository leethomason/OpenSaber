import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole

mat = init_material(sys.argv[1])

DZ_PLATE = 36.0
W_PLATE = 17.0
DEPTH = -14.0

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

travel(g, mat, x=0, y=-DZ_PLATE/2)
g.move(z=0)
rectangleTool(g, mat, DEPTH, W_PLATE, DZ_PLATE, W_PLATE/2, "bottom", "inner", fill=False, adjust_trim=True)
g.move(z=15)
g.spindle()




