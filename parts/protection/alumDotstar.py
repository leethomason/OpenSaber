# aligned at the alignment ring,
# between 3rd & 4th
# has the z problem - hover above a bit on z

import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G

DEPTH = -4.5
DDOT = 4.0
DZ = 21.0 + DDOT

mat = init_material(sys.argv[1])

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

g.move(z=2)

g.feed(mat['feed_rate'])
g.spindle('CW', mat['spindle_speed'])

g.move(y = -7*2.5 - DDOT/2)
g.move(z=0)

rectangleTool(g, mat, DEPTH, DDOT, DZ, DDOT/2, "bottom", "inner", fill=False, adjust_trim=True)

g.move(z=16)
g.spindle()

