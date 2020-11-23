import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole

mat = init_material(sys.argv[1])

DZ_PLATE = 36.0
W_PLATE = 19.0
DZ_PORT, DZ_BOLT, DZ_SWITCH = 18, 8, 29.3
H_PLATE = 7.575

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

depth = -(25.4/4 + 1.0)
sink = -1.0

LENGTH = 39.25
WIDTH = 16.5

PORT = 8
BOLT = 21
SWITCH = 32.25
# Weird and tweaked because of a 3.3mm bit (grr)
TRIM = -0.04

TROUGH = 11.0

travel(g, mat, y=PORT)
g.move(z=0)
hole(g, mat, depth, d=11.0)

travel(g, mat, y=BOLT)
hole(g, mat, depth, d=4.4)
hole(g, mat, -2.0, d=8.0)

travel(g, mat, y=SWITCH)
hole(g, mat, depth, d=8.2)
travel(g, mat, y=SWITCH - 11.0/2)
rectangleTool(g, mat, sink, TROUGH, LENGTH - SWITCH + 16.0, TROUGH/2, "bottom", "inner", fill=True, adjust_trim=True)

travel(g, mat, y=TRIM/2)
rectangleTool(g, mat, depth, WIDTH - TRIM, LENGTH - TRIM, 3.175/2, "bottom", "outer", fill=False, adjust_trim=True)
