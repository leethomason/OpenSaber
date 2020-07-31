import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole, hole_or_drill

# positied so that bit is centered at top.

DOTSTAR = -28.4


mat = init_material(sys.argv[1])
d = float(sys.argv[2])

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

g.feed(mat['feed_rate'])
g.spindle('CW', mat['spindle_speed'])

for p in range(0, 4):
    for i in range(0, 4):
        g.move(z=2)
        g.move(y=DOTSTAR + i * 7.0)
        g.move(z=0)

        hole_or_drill(g, mat, -2.0 * (p+1)/4, d/2)

g.move(z=15)
g.spindle()

