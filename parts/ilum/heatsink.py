import sys
import math
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole
from plane import plane

mat = init_material(sys.argv[1])
mat1 = None

if len(sys.argv) > 2:
    mat1 = init_material(sys.argv[2])

H = 4.0
H_FLOOR = 1.0

STOCK = 25.95   # HDPE
#STOCK = 6.0    # acrylic?

PLANE_DEPTH = -(STOCK - H) 
D = 25.2
DEPTH_EPS = -0.2

R_HOLE = 8.92
D_HOLE = 4.4
D_INNER = 12.0

PLANE_XY = D + mat['tool_size']

# origin is at the center of the part

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()
g.move(z=0)
plane(g, mat, PLANE_DEPTH, -PLANE_XY/2, -PLANE_XY/2, PLANE_XY/2, PLANE_XY/2)

g.move(x=0, y=0)

if (mat1 != None):
    tool_change(g, mat1, 1)
    mat = mat1

g.move(z=-PLANE_DEPTH)

for r in range(6):
    theta = r * math.pi * 2.0 / 6.0
    x = math.cos(theta) * R_HOLE
    y = math.sin(theta) * R_HOLE
    
    g.move(z=PLANE_DEPTH + 1.0)
    g.move(x=x, y=y)
    g.move(z=PLANE_DEPTH)
    hole(g, mat, -H + DEPTH_EPS, d=D_HOLE)

g.move(z=PLANE_DEPTH + 1.0)
g.move(x=0, y=0)
g.move(z=PLANE_DEPTH)
hole(g, mat, -(H-H_FLOOR) + DEPTH_EPS, d=D_INNER)

hole(g, mat, -H  + DEPTH_EPS, d=D, offset="outside", fill=False)

g.move(z=10.0)
g.spindle()
