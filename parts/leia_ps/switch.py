import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from rectangle import rectangle
from mecode import G
from hole import hole, drill

M_BOLT_0, M_SWITCH, M_PORT, M_BOLT_1 = 124.7, 139.7, 157.8, 173.875
M_PCB, DX_PCB, DZ_PCB = 119.147, 14, 58.7279

JOINT = 6.0
DX_JOINT = 3.0
DZ_JOINT = 5.0

COPPER_DEPTH = -0.4
CUT_DEPTH = -2.0
HEADER = 2.54

mat = init_material("np883-fr-1.0")
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)

tool_size = mat["tool_size"]
half_tool = tool_size / 2

g.absolute()

def pad(x, y, dir):
    travel(g, mat, x=x, y=y)
    g.move(z=0)
    drill(g, mat, CUT_DEPTH)

    travel(g, mat, x=x, y=y + dir * HEADER)
    g.move(z=0)
    drill(g, mat, CUT_DEPTH)

    travel(g, mat, x=x, y=y - dir*HEADER/2)
    align = "bottom"
    if dir < 0:
        align = "top"
    rectangleTool(g, mat, COPPER_DEPTH, HEADER, HEADER*2, 0, align, "outer")

# Post
travel(g, mat, x=M_BOLT_0 - M_PCB, y=0)
g.move(z=0)
hole(g, mat, CUT_DEPTH, d=3.0, fill=False)

# switch
pad(M_SWITCH - M_PCB - 2.3, 3.5, 1)
pad(M_SWITCH - M_PCB + 2.3, 3.5, 1)
pad(M_SWITCH - M_PCB - 2.3, -3.5, -1)
pad(M_SWITCH - M_PCB + 2.3, -3.5, -1)

# port
travel(g, mat, x=M_PORT - M_PCB, y=0)
g.move(z=0)
hole(g, mat, CUT_DEPTH, d=8.0, fill=False)

# Post
travel(g, mat, x=M_BOLT_1 - M_PCB, y=0)
g.move(z=0)
hole(g, mat, CUT_DEPTH, d=3.0, fill=False)

# joint
travel(g, mat, x=JOINT + DZ_JOINT/2, y=DX_PCB/2 - DX_JOINT - half_tool)
g.move(z=0)
rectangle(g, mat, CUT_DEPTH, DZ_JOINT - tool_size, DX_JOINT + tool_size, 0, "bottom")

travel(g, mat, x=JOINT + DZ_JOINT/2, y=-DX_PCB/2 - half_tool)
g.move(z=0)
rectangle(g, mat, CUT_DEPTH, DZ_JOINT - tool_size, DX_JOINT + tool_size, 0, "bottom")

# outside
travel(g, mat, x=0, y=0)
g.move(z=0)
rectangleTool(g, mat, CUT_DEPTH, DZ_PCB, DX_PCB, 0, "left", "outer")

g.move(z=15)

