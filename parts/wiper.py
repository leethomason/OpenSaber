import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from rectangle import rectangle
from mecode import G
from hole import hole, drill

mat = init_material("np883-evafoam-1.0")
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)

DEPTH = -6.0

g.move(z=0)
hole(g, mat, DEPTH, d=16.2, offset='inside', fill=False)

g.move(z=0)
hole(g, mat, DEPTH, d=21.4, offset='outside', fill=False)

g.move(z=15.0)
