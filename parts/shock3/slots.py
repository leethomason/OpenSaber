import sys
from mecode import GMatrix, G
from material import init_material
from capsule import capsule
from utility import *
from rectangle import rectangle

# cut the shock3
# origin centered on switch, at first ridge

CUT_DEPTH = -6

# Basic
DIAMETER = 18.0
INSET_L = 40.0
INSET_CENTER_M = -63.1 / 2
SPACE = 3.2

CRYSTAL_0_SECTION_M = 13.15
CRYSTAL_0_SECTION_L = 19.15
CRYSTAL_0_L = CRYSTAL_0_SECTION_L - SPACE * 2

CRYSTAL_1_SECTION_M = 34.6
CRYSTAL_1_SECTION_L = 13.67
CRYSTAL_1_L = CRYSTAL_1_SECTION_L - SPACE * 2

OLED_M = 67.3 + 10.0
OLED_L = 24.6
OLED_W = 8.3

# GMatrix writes all of x, y, z, so moves must be fully specified.
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None, print_lines=False)
mat = init_material(sys.argv[1])

# Derived
INSET_M = INSET_CENTER_M - INSET_L / 2
CRYSTAL_0_M = CRYSTAL_0_SECTION_M + CRYSTAL_0_SECTION_L - CRYSTAL_0_SECTION_L
CRYSTAL_1_M = CRYSTAL_1_SECTION_M + CRYSTAL_1_SECTION_L - CRYSTAL_1_SECTION_L
R = DIAMETER / 2

tool_size = mat['tool_size']
half_tool = tool_size / 2

#############################

g.absolute()
g.feed(mat['feed_rate'])

g.move(z=CNC_TRAVEL_Z)
g.move(x=-R + half_tool, y=INSET_M + half_tool)
g.move(z=0)
rectangle(g, mat, CUT_DEPTH, DIAMETER - tool_size, INSET_L - tool_size)

g.move(z=CNC_TRAVEL_Z)
g.move(x=-R + half_tool, y=CRYSTAL_0_M + half_tool)
g.move(z=0)
rectangle(g, mat, CUT_DEPTH, DIAMETER - tool_size, CRYSTAL_0_L - tool_size)

g.move(z=CNC_TRAVEL_Z)
g.move(x=-R + half_tool, y=CRYSTAL_1_M + half_tool)
g.move(z=0)
rectangle(g, mat, CUT_DEPTH, DIAMETER - tool_size, CRYSTAL_1_L - tool_size)

g.move(z=CNC_TRAVEL_Z)
g.move(x=-OLED_W/2 + half_tool, y=OLED_M + half_tool)
g.move(z=0)
rectangle(g, mat, CUT_DEPTH, OLED_W - tool_size, OLED_L - tool_size)

g.move(z=CNC_TRAVEL_Z)
g.spindle()
g.move(x=0, y=0)
g.move(z=0)



