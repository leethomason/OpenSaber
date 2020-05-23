import sys
from nanopcb import nanopcb
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole

CUT_DEPTH = -1.8
PCB_DEPTH = -0.3
UNIT = 2.54
R_BOLT = UNIT * 3.0 + 0.5
D_M2 = 2.05

D_COUPLER_OUTER = 20.24
D_M2_HEAD = 4.0
R_M2_FASTENER_BOLT = (D_COUPLER_OUTER - D_M2_HEAD) / 2

D_COUPLER_DISC = 22.0


# 0 is the aft side
# 1 the fore side

aftSide = sys.argv[1] == '0'

mat = init_material("np883-fr-1.0")
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)

cx, cy = nanopcb("powerCoupler.txt", g, mat, PCB_DEPTH, CUT_DEPTH, 
    False,  # don't cut
    False, True, False, False)

g.absolute()

HOLES = [0, 180, 45, 180-45, -45, 180+45]

if aftSide:
    for h in HOLES:
        sx = math.cos(h * 2.0 * math.pi / 360)
        sy = math.sin(h * 2.0 * math.pi / 360)

        travel(g, mat, x=cx/2 + R_BOLT * sx, y=cy/2 + R_BOLT * sy)
        g.move(z=0)
        hole(g, mat, CUT_DEPTH, d=D_M2)

    # add the top/bottom (in this coordinate system) mounting holes that
    # attach to the carriage.

    # r = D_VENT/2 - D_M2_HEAD/2
    r = R_M2_FASTENER_BOLT
    travel(g, mat, x=cx/2, y=cy/2 + r)
    g.move(z=0)
    hole(g, mat, CUT_DEPTH, d=D_M2)

    travel(g, mat, x=cx/2, y=cy/2 - r)
    g.move(z=0)
    hole(g, mat, CUT_DEPTH, d=D_M2)

g.move(z=2)
g.move(x=cx/2, y=cy/2)

if aftSide:
    hole(g, mat, CUT_DEPTH, d=D_COUPLER_OUTER, offset="outside", fill=False, z=0, return_center=False)

else:
    hole(g, mat, CUT_DEPTH, d=D_COUPLER_DISC, offset="outside", fill=False, z=0, return_center=False)

g.move(z=10)
g.spindle()


