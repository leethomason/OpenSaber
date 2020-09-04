import sys
import math
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from rectangle import rectangle
from mecode import G
from hole import hole

# positied so that bit is centered at bottom of copper
DEPTH = -6.0
D = 11.0 - 3.175
SCALE = 2.2
C = 23.47

def rel_point(angle):
    x = math.sin(angle) * D
    y = -math.cos(angle) * D * SCALE
    return x, y

mat = init_material(sys.argv[1])

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

g.move(z=2)
x, y = rel_point(0)
g.move(y=y + C)

g.feed(mat['feed_rate'])
g.spindle('CW', mat['spindle_speed'])

g.move(z=0)

def path(g, total_plunge, plunge):
    STEPS = 120
    for s in range(0, STEPS):
        x, y = rel_point(s * math.pi * 2.0 / STEPS)
        g.move(x=x, y=y + C, z=total_plunge + plunge * (s+1) / STEPS)

steps = calc_steps(DEPTH, -mat['pass_depth'])
run_3_stages_abs(path, g, steps)

g.move(z=15)
g.spindle()

