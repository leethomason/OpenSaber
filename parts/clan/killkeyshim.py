import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole
from drill import drill

STOCK = 12.75
D = 10.5
T = 6.2 + 0.5

# killkey.py material height

mat = init_material(sys.argv[1])

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

# center pin
g.move(z=0)
hole(g, mat, -STOCK, d=5.52)

# flatten
g.move(z=0)
hole(g, mat, T - STOCK, d=D, offset="outside", fill=True)

# cut out
g.move(z=T - STOCK)
hole(g, mat, -T, d=D, offset="outside", fill=False)
