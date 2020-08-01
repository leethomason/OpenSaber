import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole

# positied so that bit is centered at top.

'''
ECHO: "CENTER Aluminum cut, from top of section. On tool centers."
ECHO: "P0", 0, -45.0125
ECHO: "P1", 8, -31.13
ECHO: "P2", 0, -17.2475
ECHO: "P3", -8, -31.13
'''
X0 = -8
X1 = 8
Y0 = -45.0125
Y1 = -31.13
Y2 = -17.2475

M_SWITCH = 104.4
M_PORT = M_SWITCH - 16.0
M_ALIGN = M_SWITCH
D_CAPSULE = 12.0
D_BOLT = 8.0

DEPTH = -6.0
BODY = 7*25.4

mat = init_material(sys.argv[1])
frontFace = int(sys.argv[2])

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

g.move(z=2)
g.move(y=Y2)

g.feed(mat['feed_rate'])
g.spindle('CW', mat['spindle_speed'])

g.move(z=0)

def path(g, total_plunge, plunge):
    g.move(x=X1, y=Y1, z=total_plunge + plunge * 0.25)
    g.move(x=0, y=Y2, z=total_plunge + plunge * 0.50)
    g.move(x=X0, y=Y1, z=total_plunge + plunge * 0.75)
    g.move(x=0, y=Y0, z=total_plunge + plunge)

steps = calc_steps(DEPTH, -mat['pass_depth'])
run_3_stages_abs(path, g, steps)

g.move(z=2)

if frontFace == 1:
    g.move(y= -BODY + M_PORT - D_CAPSULE/2)
    g.move(z=0)
    rectangleTool(g, mat, DEPTH, D_CAPSULE, M_SWITCH - M_PORT + D_CAPSULE, D_CAPSULE/2, "bottom", "inner", adjust_trim=True)

if frontFace == 0:
    # plate for screw head
    g.move(y = -BODY + M_ALIGN)
    g.move(z=0)
    hole(g, mat, -1.0, d=D_BOLT)
    g.move(z=0)
    hole(g, mat, DEPTH, d=4.2)


g.move(z=15)
g.spindle()

