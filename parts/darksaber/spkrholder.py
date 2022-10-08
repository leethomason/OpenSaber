import sys
from nanopcb import nanopcb
from utility import *
from material import init_material
from rectangle import rectangle
from mecode import G
from hole import hole

CUT_DEPTH = -3.0
PCB_DEPTH = -0.3
D_M2 = 2.05
DRILL_DEPTH = CUT_DEPTH

INNER_DX = 21.5
PCB_BOLT_DX = INNER_DX - 5.0
PCB_BOLT_DY = 1.0

mat = init_material("np883-fr-1.0")
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)

nanopcb("spkrholder.txt", g, mat, PCB_DEPTH, DRILL_DEPTH, 
    False,  # don't cut
    False, 
    True, # drill, but not it is shallow on the back
    False, # flip
    False)

g.absolute()

travel(g, mat, x=PCB_BOLT_DX/2, y=PCB_BOLT_DY)
g.move(z=0)
hole(g, mat, CUT_DEPTH, d=D_M2)

travel(g, mat, x=-PCB_BOLT_DX/2, y=PCB_BOLT_DY)
g.move(z=0)
hole(g, mat, CUT_DEPTH, d=D_M2)

travel(g, mat, x=0, y=-2)
rectangle(g, mat, CUT_DEPTH, INNER_DX + mat['tool_size'], 10, 0, "bottom")

g.absolute()
g.move(z=10)
g.spindle()

