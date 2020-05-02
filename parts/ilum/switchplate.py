#
# ECHO: "Plate cut, LEN", 39.25
# ECHO: "width", 16.5
# ECHO: "PORT", 8, "BOLT", 21, "SWITCH", 32.25
# 

import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole

#mat = init_material("np883-aluminum-3.175")
mat = init_material(sys.argv[1])
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)

depth = -(25.4/4 + 0.4)
sink = -1.0

length = 39.25
width = 16.5
port = 8
bolt = 21
switch = 32.25

travel(g, mat, y=port)
g.move(z=0)
hole(g, mat, depth, d=11.0)

travel(g, mat, y=bolt)
hole(g, mat, depth, d=4.2)  # FIXME diameter?
hole(g, mat, sink, d=8.0)

travel(g, mat, y=switch)
hole(g, mat, depth, d=8.0) # FIXME diameter?
travel(g, mat, y=switch - 11.0/2)
rectangleTool(g, mat, sink, width, length - switch + 16.0, width/2, "bottom", "inner", fill=True, adjust_trim=True)

travel(g, mat, y=0)
rectangleTool(g, mat, depth, width, length, 3.175/2, "bottom", "outer")