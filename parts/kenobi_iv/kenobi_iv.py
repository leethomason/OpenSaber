from material import init_material
from mecode import G
from drill import drill
from hole import hole
from utility import CNC_TRAVEL_Z
from rectangle import rectangle

mat = init_material("np883-fr-1.0")
tool_size = 1.0
half_tool = tool_size / 2
cut_depth = -5.0

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None, print_lines=False)
g.absolute()
g.move(z=CNC_TRAVEL_Z)
g.spindle('CW', mat['spindle_speed'])

# Drill holes for the tactile switch.
holes = [
    [15.25, 14.75],
    [19.85, 14.75],
    [15.25,  8.26],
    [19.85,  8.26]
]

for h in holes:
    g.move(x=h[0], y=h[1])
    drill(g, mat, cut_depth)

# Mounting holes.
mount = [
    [ 2.54,   3.175],
    [22.225,  3.175],
    [ 2.54,  19.685],
    [22.225, 19.685]
]
for m in mount:
    g.move(x=m[0], y=m[1])
    hole(g, mat, cut_depth, 2.1/2)

# Power port.
g.move(x=7.62, y=11.43)
hole(g, mat, cut_depth, 8.1/2)

# Board outline.
g.move(x=-half_tool, y=-half_tool)
g.move(z=0)
rectangle(g, mat, cut_depth, 24.76 + tool_size, 22.86 + tool_size)
g.move(z=CNC_TRAVEL_Z)

g.spindle()
