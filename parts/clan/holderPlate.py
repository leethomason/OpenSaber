import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole

ITOMM = 25.4

D = 1.45 * ITOMM
Y0 = 1.45 * ITOMM
Y1 = 0.20 * ITOMM
Y = Y0 + Y1
X = 2.9 * ITOMM
X1 = 1.1 * ITOMM
X0 = (X - X1) / 2
Z = 1.0 * ITOMM

mat = init_material(sys.argv[1])

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

ht = mat['tool_size'] / 2
travel(g, mat, x=0, y=-Y0-ht)
g.move(z=0)


def absPath(g, total_plunge, plunge):
    g.move(z=total_plunge)
    g.feed(mat['plunge_rate'])
    g.move(z=total_plunge + plunge)
    g.feed(mat['feed_rate'])

    g.move(x=X0-ht)
    g.move(y=-Y0)
    g.move(y=-Y-ht)

    g.move(x=X-X0+ht)

    g.move(y=-Y0)
    g.move(y=-Y0-ht)
    g.move(x=X+ht)

    g.move(z=1)
    g.move(x=0, y=-Y0-ht)
    g.move(z=0)


steps = calc_steps(-0.6*ITOMM, -mat['pass_depth'])
run_3_stages_abs(absPath, g, steps)

g.move(z=10)
g.spindle()

