import sys
from mecode import GMatrix, G
from material import init_material
from capsule import capsule
from utility import *
from rectangle import rectangle

# cut the metal cowling for the crystal
# origin at center left line of metal

WIDTH_METAL = 24.0
LENGTH_METAL = 44.0
WINDOW_X0 = 9.1
WINDOW_X1 = 16.37
WINDOW_X2 = 25.1
WINDOW_X3 = 37.85
WINDOW_W = 18.5

SETBACK = 3.0   # how far to hide cuts
LEDGE = 3.2     # how far the lower ledge juts into the view
RAD = 1.6       # corner for little window
OLED_W = 8.3

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None, print_lines=False)
mat = init_material(sys.argv[1])
CUT_DEPTH = float(sys.argv[2])

tool_size = mat['tool_size']
half_tool = tool_size / 2

#############################
# little window on left
# big window on right

travel(x=WINDOW_X0 - SETBACK)

def absPath(g, plunge, total_plunge):
    g.move(y=OLED_W/2 - half_tool)
    g.move(x=(WINDOW_X1 - WINDOW_X0)/2 - RAD)

    g.relative()
    g.arc2(x=RAD, y=RAD, i=0, j=RAD, direction="CCW")
    g.absolute() 
    
    g.move(y=WINDOW_W/2 - LEDGE - half_tool)
    g.move(x=WINDOW_X3 + SETBACK)
    g.move(y=-WINDOW_W/2 + LEDGE - half_tool)
    g.move(x=(WINDOW_X1 - WINDOW_X0)/2 + half_tool)
    g.move(y=-OLED_W/2 - RAD)

    g.relative()
    g.arc2(x=-RAD, y=RAD, i=-RAD, j=0, direction="CW")
    g.absolute() 

    g.move(x=WINDOW_X0 - SETBACK)
    g.move(y=0)

run_3_stages_abs(path, g, CUT_DEPTH)
