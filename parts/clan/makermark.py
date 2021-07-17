import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole
from drill import drill

# makermark.py material section oddDepth

mat = init_material(sys.argv[1])
section = float(sys.argv[2])
oddDepth = float(sys.argv[3])
D = 2.0
STEP = 3.0
depth = -0.5

if (oddDepth > 0): raise RuntimeError("oddDepth greater than 0")

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

space = (section - STEP*3) / 2

for i in range(0, 4):
    d = depth
    if i == 2:
        if oddDepth >= 0:
            continue
        d = oddDepth

    g.feed(mat['feed_rate'])
    g.move(z=CNC_TRAVEL_Z)
    g.move(x=0, y=-(space + STEP*i))
    g.move(z=0)
    hole(g, mat, d, d=D)

g.move(z=10)
