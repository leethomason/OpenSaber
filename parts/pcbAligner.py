import sys
from rectangleTool import rectangleTool
from nanopcb import nanopcb
from utility import *
from material import init_material
from rectangle import rectangle
from mecode import G
from hole import hole

W_0805 = 2.0
H_0805 = 1.5
W_AND = 4.0     # FIXME
H_AND = 3.0     # FIXME

comp = [
    {"name": "amp10u", "x": 4.76, "y": 12.7, "dx": W_0805, "dy": H_0805 },
    {"name": "amp100n", "x": 4.76, "y": 10.795, "dx": W_0805, "dy": H_0805 },
    {"name": "amp", "x": 4.9, "y": 7.1, "dx": 3.0, "dy": 3.0},
    {"name": "10k", "x": 10.795, "y": 7.935, "dx": H_0805, "dy": W_0805},
    {"name": "47k", "x": 12.7, "y": 6.03, "dx": H_0805, "dy": W_0805},
    {"name": "dotstar10u", "x": 17.78, "y": 8.57, "dx": H_0805, "dy": W_0805 },
    {"name": "dotstar100n", "x": 20.32, "y": 8.57, "dx": H_0805, "dy": W_0805 },
    {"name": "led", "x": 17.78, "y": 3.49, "dx": H_0805, "dy": W_0805 },
    {"name": "tie", "x": 20.32, "y": 3.49, "dx": H_0805, "dy": W_0805 },
    {"name": "and1", "x": 25.955, "y": 11.428, "dx": W_AND, "dy": H_AND },
    {"name": "and2", "x": 25.955, "y": 7.3, "dx": W_AND, "dy": H_AND },
]

def cutComp(g, mat, depth, x, y, dx, dy):
    half_tool = mat['tool_size'] / 2
    travel(g, mat, x=x, y=y)
    g.move(z=0)

    rectangleTool(g, mat, depth, dx, dy, 0, "center", "inner", True)

    if dy > dx:
        # x-axis handles
        g.move(z=0)
        g.move(x=x-dx/2, y=y+dy/2-half_tool)
        rectangle(g, mat, depth, dx, 0, 0, "left")

        g.move(z=0)
        g.move(x=x-dx/2, y=y-dy/2+half_tool)
        rectangle(g, mat, depth, dx, 0, 0, "left")

    else:
        # y-axis handles
        g.move(z=0)
        g.move(x=half_tool, y=y-dy/2)
        rectangle(g, mat, depth, 0, dy, 0, "bottom")

        g.move(z=0)
        g.move(x=half_tool, y=y-dy/2)
        rectangle(g, mat, depth, 0, dy, 0, "bottom")

    g.move(z=0)


print("Mat:", sys.argv[1])
print("Depth:", sys.argv[2])

mat = init_material(sys.argv[1])
DEPTH = float(sys.argv[2])

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

for c in comp:
    cutComp(g, mat, DEPTH, c["x"], c["y"], c["dx"], c["dy"])

