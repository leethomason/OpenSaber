import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole

mat = init_material(sys.argv[1])
side = int(sys.argv[2])

T_STOCK = 26.0
H_HEAD = 3.0 # 4.5
H_PIN = 6.5
D_HEAD = 6.2
D_PIN = 4.2
H_INSERT = 2.0
D_INSERT = 4.0

toolSize = mat["tool_size"]

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

# cut pin from side 0
if side == 0:
    # cut down to pin
    g.move(x=0, y=0)
    g.move(z=0)
    hole(g, mat, -T_STOCK + H_HEAD + H_PIN, d=D_PIN + toolSize, fill=True)
    # cut around pin
    g.move(x=0, y=0)
    g.move(z=0)
    hole(g, mat, -T_STOCK + H_HEAD, d=D_PIN + toolSize, offset="middle", fill=False)

# cut head from side 1
if side == 1:
    # cut center hole
    g.move(x=0, y=0)
    g.move(z=0)
    hole(g, mat, -H_INSERT, d=D_INSERT)
    # cut the head free
    g.move(x=0, y=0)
    g.move(z=0)
    hole(g, mat, -H_HEAD - 1.0, d=D_HEAD + toolSize, offset="middle", fill=False)

g.move(z=30.0)