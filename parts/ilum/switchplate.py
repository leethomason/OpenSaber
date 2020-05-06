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

mat = init_material(sys.argv[1])
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

depth = -(25.4/4 + 0.4)
sink = -1.0

LENGTH = 39.25
WIDTH = 16.5

PORT = 8
BOLT = 21
SWITCH = 32.25
TRIM = 0.2

TROUGH = 11.0

travel(g, mat, y=PORT)
g.move(z=0)
hole(g, mat, depth, d=11.0)

travel(g, mat, y=BOLT)
hole(g, mat, depth, d=4.4)
hole(g, mat, sink, d=8.0)

travel(g, mat, y=SWITCH)
hole(g, mat, depth, d=8.2)
travel(g, mat, y=SWITCH - 11.0/2)
rectangleTool(g, mat, sink, TROUGH, LENGTH - SWITCH + 16.0, TROUGH/2, "bottom", "inner", fill=True, adjust_trim=True)

travel(g, mat, y=TRIM/2)
rectangleTool(g, mat, depth, WIDTH - TRIM, LENGTH - TRIM, 3.175/2, "bottom", "outer", fill=False, adjust_trim=True)
