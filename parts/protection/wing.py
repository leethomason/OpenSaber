import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole

# positied so that bit is centered at top.

'''
ECHO: "WING Aluminum cut, from top of saber body. On tool center."
ECHO: "Right"
ECHO: "P0", -6, -48.0125
ECHO: "P1", 4, -41.0125
ECHO: "P2", 7, -31.0125

ECHO: "Left"
ECHO: "P0", -6, -48.0125
ECHO: "P1", -16, -41.0125
ECHO: "P2", -19, -31.0125
'''

M_SWITCH = 104.4
M_PORT = M_SWITCH - 16.0
M_ALIGN = M_SWITCH
D_CAPSULE = 12.0
D_BOLT = 8.0

DEPTH = -6.0
BODY = 7*25.4

mat = init_material(sys.argv[1])
right = int(sys.argv[2])

if right == 1:
    X0 = -6
    Y0 = -48.0125
    X1 = 4
    Y1 = -41.0125
    X2 = 7
    Y2 = -31.0125

if right == 0:
    X0 = 6
    Y0 = -48.0125
    X1 = -4
    Y1 = -41.0125
    X2 = -7
    Y2 = -31.0125

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

g.move(z=2)
g.move(x=X0, y=Y0)

g.feed(mat['feed_rate'])
g.spindle('CW', mat['spindle_speed'])

g.move(z=0)

def path(g, total_plunge, plunge):
    g.move(x=X1, y=Y1, z=total_plunge + plunge * 0.33)
    g.move(x=X2, y=Y2, z=total_plunge + plunge * 0.66)
    g.move(x=X0, y=Y0, z=total_plunge + plunge)
    

steps = calc_steps(DEPTH, -mat['pass_depth'])
run_3_stages_abs(path, g, steps)

g.move(z=15)
