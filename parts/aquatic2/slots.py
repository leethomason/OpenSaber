import sys
from mecode import GMatrix, G
from material import init_material
from capsule import capsule
from utility import *

GRIP_LEN = 58.5
GRIP_CUT = 28
CUT_WIDTH = 12

BACK_LOC = 23.8
BACK_CUT = 22
CUT_DEPTH = -6

# GMatrix writes all of x, y, z, so moves must be fully specified.
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None, print_lines=False)

mat = init_material(sys.argv[1])

g.absolute()
g.feed(mat['feed_rate'])

g.move(z=CNC_TRAVEL_Z)
g.move(y=GRIP_LEN/2)
g.move(z=0)

capsule(g, mat, CUT_DEPTH, GRIP_CUT, CUT_WIDTH, None, True, 'y')

g.move(z=CNC_TRAVEL_Z)
g.move(y=-BACK_LOC + BACK_CUT / 2)
g.move(z=0)
capsule(g, mat, CUT_DEPTH, BACK_CUT, CUT_WIDTH, None, True, 'y')

g.move(z=CNC_TRAVEL_Z)
g.spindle()
g.move(x=0, y=0)
