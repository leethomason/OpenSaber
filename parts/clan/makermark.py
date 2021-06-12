import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole
from drill import drill

# makermark.py material depth section

mat = init_material(sys.argv[1])
section = float(sys.argv[2])
D = 2.0
STEP = 3.0
depth = -0.5

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

space = (section - STEP*3) / 2

for i in range(0, 4):
    if i == 2:
        continue

    g.feed(mat['feed_rate'])
    g.move(z=CNC_TRAVEL_Z)
    g.move(x=0, y=space + STEP*i)
    g.move(z=0)
    hole(g, mat, depth, d=D)

g.move(z=10)
