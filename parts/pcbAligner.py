import sys
from rectangleTool import rectangleTool
from nanopcb import nanopcb
from utility import *
from material import init_material
from rectangle import rectangle
from mecode import G
from hole import hole
from drill import drill

L_0805 = 2.0
W_0805 = 1.5
W_AND = 4.0     # FIXME
H_AND = 3.0     # FIXME
W_MOSFET = 5.0  # FIXME
H_MOSFET = 3.0  # FIXME

comp = [
    {"name": "amp10u", "x": 4.76, "y": 12.7, "dx": L_0805, "dy": W_0805 },
    {"name": "amp100n", "x": 4.76, "y": 10.795, "dx": L_0805, "dy": W_0805 },
    {"name": "amp", "x": 4.9, "y": 7.1, "dx": 3.0, "dy": 3.0},
    {"name": "10k", "x": 10.795, "y": 7.935, "dx": W_0805, "dy": L_0805},
    {"name": "47k", "x": 12.7, "y": 6.03, "dx": W_0805, "dy": L_0805},
    {"name": "dotstar10u", "x": 17.78, "y": 8.57, "dx": W_0805, "dy": L_0805 },
    {"name": "dotstar100n", "x": 20.32, "y": 8.57, "dx": W_0805, "dy": L_0805 },
    {"name": "led", "x": 17.78, "y": 3.49, "dx": W_0805, "dy": L_0805 },
    {"name": "tie", "x": 20.32, "y": 3.49, "dx": W_0805, "dy": L_0805 },
    {"name": "and1", "x": 25.955, "y": 11.428, "dx": W_AND, "dy": H_AND },
    {"name": "and2", "x": 25.955, "y": 7.3, "dx": W_AND, "dy": H_AND },
    {"name": "6d10u", "x": 32.067, "y": 9.205, "dx": W_0805, "dy": L_0805 },
    {"name": "6b100n", "x": 33.973, "y": 9.205, "dx": W_0805, "dy": L_0805 },
    {"name": "6d", "x": 33.02, "y": 5.08, "dx": 3.0, "dy": 2.5},
    {"name": "red-ring", "x": 39.685, "y": 12.754, "dx": L_0805, "dy": W_0805 },
    {"name": "red-tie", "x": 39.685, "y": 10.849, "dx": L_0805, "dy": W_0805 },
    {"name": "green-ring", "x": 39.685, "y": 8.944, "dx": L_0805, "dy": W_0805 },
    {"name": "green-tie", "x": 39.685, "y": 6.985, "dx": L_0805, "dy": W_0805 },
    {"name": "blue-ring", "x": 39.685, "y": 5.08, "dx": L_0805, "dy": W_0805 },
    {"name": "blue-tie", "x": 39.685, "y": 3.175, "dx": L_0805, "dy": W_0805 },
    {"name": "mosfet-red", "x": 46.533, "y": 12.009, "dx": W_MOSFET, "dy": H_MOSFET },
    {"name": "mosfet-green", "x": 46.534, "y": 8.577, "dx": W_MOSFET, "dy": H_MOSFET },
    {"name": "mosfet-blue", "x": 46.534, "y": 5.283, "dx": W_MOSFET, "dy": H_MOSFET },
    {"name": "red-res-1", "x": 50.163, "y": 12.283, "dx": L_0805, "dy": W_0805 },
    {"name": "red-res-2", "x": 50.163, "y": 10.378, "dx": L_0805, "dy": W_0805 },
    {"name": "green-res-1", "x": 50.163, "y": 8.156, "dx": L_0805, "dy": W_0805 },
    {"name": "green-res-2", "x": 50.163, "y": 6.251, "dx": L_0805, "dy": W_0805 },
    {"name": "blue-res-1", "x": 50.163, "y": 4.028, "dx": L_0805, "dy": W_0805 },
    {"name": "blue-res-2", "x": 50.163, "y": 2.213, "dx": L_0805, "dy": W_0805 },
]

drillHoles = [
    {"x": 0, "y": 0},
    {"x": 53.34, "y": 0},
    {"x": 53.34, "y": 15.24},
    {"x": 0, "y": 15.24},
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
        g.move(x=x-half_tool, y=y-dy/2)
        rectangle(g, mat, depth, 0, dy, 0, "bottom")

        g.move(z=0)
        g.move(x=x+half_tool, y=y-dy/2)
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

for d in drillHoles:
    travel(g, mat, x=d["x"], y=d["y"])    
    g.move(z=0)
    drill(g, mat, DEPTH)

travel(g, mat, x=-1.27, y=-1.27 + 17.78/2)
g.move(z=0)
rectangle(g, mat, DEPTH, 55.88 + mat['tool_size'], 17.78 + mat['tool_size'], 0, "left")
g.move(z=20)


