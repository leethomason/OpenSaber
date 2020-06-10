import sys
import math
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole
from plane import plane

mat = init_material(sys.argv[1])

STOCK = 6.35      # HDPE
H = 3.5
H_FLOOR = 1.5   # needs 2mm; to H_FLOOR + 2 <= H
D_HEATSINK = 20.2

# origin is at the center of the part

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()
g.move(z=0)

hole(g, mat, -(STOCK - H), d=D_HEATSINK, offset="inside")

g.move(z=10.0)
g.spindle()
