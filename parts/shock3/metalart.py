import sys
from mecode import GMatrix, G
from material import init_material
from utility import *
from rectangleTool import rectangleTool

# cut the metal cowling for the crystal
# origin at center left line of metal

WIDTH_METAL = 20.0
LENGTH_METAL = 44.0
WINDOW_X0 = 9.1
WINDOW_X1 = 16.37
WINDOW_X2 = 25.1
WINDOW_X3 = 37.85
WINDOW_W = 18.5

SETBACK = 2.5   # how far to hide cuts
LEDGE = 3.2     # how far the lower ledge juts into the view
RAD = 1.0       # corner for little window
OLED_W = 8.3

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None, print_lines=False)
mat = init_material(sys.argv[1])
CUT_DEPTH = float(sys.argv[2])

nomad_header(g, mat, CNC_TRAVEL_Z)
g.move(z=0)

tool_size = mat['tool_size']
half_tool = tool_size / 2

#############################
# little window on left
# big window on right


g.absolute()
travel(g, mat, x=WINDOW_X0 - SETBACK)
X_MARK = (WINDOW_X0 + WINDOW_X1) / 2 - 1.0

def absPath(g, total_plunge, plunge):
    g.move(y=OLED_W/2 - half_tool)

    g.move(x=X_MARK - RAD + half_tool)

    g.relative()
    g.arc2(x=RAD, y=RAD, i=0, j=RAD, direction="CCW")
    g.absolute() 
    
    g.move(y=WINDOW_W/2 - LEDGE - half_tool)
    g.move(x=WINDOW_X3 + SETBACK, z=total_plunge + plunge/2)
    g.move(y=-WINDOW_W/2 + LEDGE + half_tool)

    g.move(x=X_MARK + half_tool, z=total_plunge+plunge)
    g.move(y=-OLED_W/2 - RAD + half_tool)

    g.relative()
    g.arc2(x=-RAD, y=RAD, i=-RAD, j=0, direction="CCW")
    g.absolute() 

    g.move(x=WINDOW_X0 - SETBACK)
    g.move(y=0)

steps = calc_steps(CUT_DEPTH, -mat['pass_depth'])
run_3_stages_abs(absPath, g, steps)
g.move(z=0)

travel(g, mat, x=0, y=0)
rectangleTool(g, mat, CUT_DEPTH, LENGTH_METAL, WIDTH_METAL, 0.5, "left", "outer")
