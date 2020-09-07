# aligned at the alignment ring,
# between 3rd & 4th
# has the z problem - hover above a bit on z

import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from hole import hole
from mecode import G

mat = init_material(sys.argv[1])

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

g.move(z=2)

g.feed(mat['feed_rate'])
g.spindle('CW', mat['spindle_speed'])

for p in range(5):
    depth = -(0.5 + p * 0.5)

    for i in range(4):
        g.move(x = -7 * 1.5 + 7*i)
        hole(g, mat, depth, d=1.6, offset="inside", z=0)
        g.move(z=2)


g.move(z=16)
g.spindle()

