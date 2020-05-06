import sys
import math
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole
from plane import plane

mat0 = init_material(sys.argv[1])
mat1 = init_material(sys.argv[2])

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat0, CNC_TRAVEL_Z)
g.absolute()
g.move(z=0)

plane(g, mat0, -2, 0, 0, 10, 10)

tool_change(g, mat1, 1)

g.move(x=2, y=2)
g.move(z=-2)
plane(g, mat1, -2, 0, 0, 6, 6)
