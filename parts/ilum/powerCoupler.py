from nanopcb import nanopcb
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole

CUT_DEPTH = -1.8
PCB_DEPTH = -0.3
UNIT = 2.54
R_BOLT = UNIT * 3

mat = init_material("np883-fr-1.0")
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)

cx, cy = nanopcb("powerCoupler.txt", g, mat, CUT_DEPTH, PCB_DEPTH, False, False, True, False, False)

g.absolute()

travel(g, mat, x=cx/2 + R_BOLT, y=cy/2)
g.move(z=0)
hole(g, mat, CUT_DEPTH, d=2.1)
travel(g, mat, x=cx/2 - R_BOLT, y=cy/2)
g.move(z=0)
hole(g, mat, CUT_DEPTH, d=2.1)

g.move(z=2)
g.move(x=cx/2, y=cy/2)
hole(g, mat, CUT_DEPTH, d=20.0, offset="outside", fill=False, z=0, return_center=False)
g.move(z=2)


