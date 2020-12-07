import sys
import math
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole
from plane import plane

mat = init_material(sys.argv[1])

H = 3.5
H_FLOOR = 1.5   # needs 2mm; to H_FLOOR + 2 <= H
STOCK = 6.35    # HDPE

D = 20.0
DEPTH_EPS = -1.0

R_HOLE = 8.0
D_HOLE = 4.4
D_INNER = 12.0

PLANE_XY = D + mat['tool_size']

# origin is at the center of the part

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
g.feed(mat['feed_rate'])
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()
g.spindle('CW', mat['spindle_speed'])
g.move(z=0)
g.move(x=0, y=0)

for r in range(6):
    theta = r * math.pi * 2.0 / 6.0
    x = math.cos(theta) * R_HOLE
    y = math.sin(theta) * R_HOLE
    
    g.move(z=1.0)
    g.move(x=x, y=y)
    g.move(z=0)
    hole(g, mat, -STOCK + DEPTH_EPS, d=D_HOLE)

g.move(z=1.0)
g.move(x=0, y=0)
g.move(z=0)
hole(g, mat, -(H-H_FLOOR) + DEPTH_EPS, d=D_INNER)

g.move(z=0)
hole(g, mat, -STOCK + DEPTH_EPS, d=D, offset="outside", fill=False)

g.move(z=10.0)
g.spindle()
