import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole

mat0 = init_material(sys.argv[1])
mat1 = init_material(sys.argv[2])

T_STOCK = 26.03
H_KEY = 21.0
H_HEAD = 6.0
D_HEAD = 10.0

H_PIN = H_KEY - H_HEAD
DEPTH_CENTER = -10.0
DEPTH_LEVEL = H_KEY - T_STOCK

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat1, CNC_TRAVEL_Z)
g.absolute()

#level to the top of the pin
g.move(x=0, y=0)
g.move(z=0)
hole(g, mat1, DEPTH_LEVEL, d=D_HEAD + 2.0 * mat1['tool_size'])

tool_change(g, mat0, 0)

g.move(x=0, y=0)
g.move(z=DEPTH_LEVEL)
hole(g, mat0, DEPTH_CENTER, d=3.0)

tool_change(g, mat1, 1)
g.move(x=0, y=0)
g.move(z=DEPTH_LEVEL)
hole(g, mat1, -H_PIN, d=5.5 + mat1['tool_size'], fill = False, offset='middle')
g.move(x=0, y=0)
g.move(z=DEPTH_LEVEL)
hole(g, mat1, -H_PIN, d=5.5 + 1.8 * mat1['tool_size'], fill = False, offset='middle')
g.move(x=0, y=0)
g.move(z=DEPTH_LEVEL)
hole(g, mat1, -H_KEY, d=10.0 + mat1['tool_size'], fill = False, offset='middle')


