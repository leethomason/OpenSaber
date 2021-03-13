import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole
from drill import drill

# makermark.py material depth slope odd_depth stride "center"/"bottom"

mat = init_material(sys.argv[1])
depth = float(sys.argv[2])
slope = float(sys.argv[3])
odd_depth = float(sys.argv[4])
stride = float(sys.argv[5])
start = sys.argv[6]

origin_y = 0
if start == "center":
    origin_y = -stride * 1.5

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

for i in range(0, 4):
    g.feed(mat['feed_rate'])
    g.move(z=CNC_TRAVEL_Z)
    g.move(x=0, y=origin_y + stride * i)
    g.move(z=1.0)

    d = depth + slope * i / 3.0

    if i == 2:
        if odd_depth < 0:
            drill(g, mat, odd_depth - 1.0)
    else:
        drill(g, mat, d - 1.0)

g.move(z=10)
