import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from rectangle import rectangle
from mecode import G
from hole import hole

# positied so that bit is centered at top.

'''
ECHO: "Brass cut, from top of brass section. On tool centers."
ECHO: "Length of section", 46.2
ECHO: "P0", 0, -31.6125
ECHO: "P1", 6, -18.73
ECHO: "P2", 0, -5.8475
ECHO: "P3", -6, -18.73
'''

DEPTH = -6.0
DZ_COPPER = 42.20
DZ_BRASS = DZ_COPPER + 4.0


mat = init_material(sys.argv[1])
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

g.move(z=2)
g.move(y=-31.6125)

g.feed(mat['feed_rate'])
g.spindle('CW', mat['spindle_speed'])

g.move(z=0)

def path(g, total_plunge, plunge):
    g.move(x=6, y=-18.73, z=total_plunge + plunge * 0.25)
    g.move(x=0, y=-5.8475, z=total_plunge + plunge * 0.50)
    g.move(x=-6, y=-18.73, z=total_plunge + plunge * 0.75)
    g.move(x=0, y=-31.615, z=total_plunge + plunge)

steps = calc_steps(DEPTH, -mat['pass_depth'])
run_3_stages_abs(path, g, steps)

g.move(z=2)
g.move(y=-DZ_BRASS - mat['tool_size']/2)
g.move(x=-6.0)
g.move(z=0)

rectangle(g, mat, DEPTH, 12.0, 0.0, 0.0, "left")

g.move(z=15)
g.spindle()

