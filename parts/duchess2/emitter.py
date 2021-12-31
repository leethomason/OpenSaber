import sys
from nanopcb import nanopcb
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole

CUT_DEPTH = -3.0
PCB_DEPTH = -0.3
UNIT = 2.54
R_BOLT = UNIT * 3.0 + 0.5
D_M2 = 2.05
D_VENT = 26.0

D_COUPLER_OUTER = 20.24
D_M2_HEAD = 4.0
R_M2_FASTENER_BOLT = (D_COUPLER_OUTER - D_M2_HEAD) / 2

DRILL_DEPTH = CUT_DEPTH

mat = init_material("np883-fr-1.0")
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)

nanopcb("emitter.txt", g, mat, PCB_DEPTH, DRILL_DEPTH, 
    False,  # don't cut
    False, 
    True, # drill, but not it is shallow on the back
    False, # flip
    False)

g.absolute()

travel(g, mat, x=0, y=(D_VENT - D_M2_HEAD)/2)
g.move(z=0)
hole(g, mat, CUT_DEPTH, d=D_M2)

travel(g, mat, x=0, y=-(D_VENT - D_M2_HEAD)/2)
g.move(z=0)
hole(g, mat, CUT_DEPTH, d=D_M2)

g.move(z=2)
g.move(x=0, y=0)

INSET = 13.5 - 0.5
D_VENT = 26.0
dhy = math.sqrt(D_VENT*D_VENT/4 - INSET*INSET/4)

bit = mat['tool_size']
dhy = dhy + bit / 4
INSET = INSET + bit

travel(g, mat, x=INSET/2, y=dhy)
g.spindle('CW', mat['spindle_speed'])
g.move(z=0)

g.relative()
def path(g, plunge, total_plunge):
    g.arc2(x=-INSET, y=0, i=-INSET/2, j=-dhy,  direction='CCW')
    g.move(x=0, y=-dhy*2, z=plunge/2)
    g.arc2(x=INSET, y=0, i=INSET/2, j=dhy, direction='CCW')
    g.move(x=0, y=dhy*2, z=plunge/2)

steps = calc_steps(CUT_DEPTH, -mat['pass_depth'])
run_3_stages(path, g, steps)

g.absolute()
g.move(z=10)
g.spindle()

