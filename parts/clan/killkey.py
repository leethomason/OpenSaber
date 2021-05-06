import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole
from drill import drill

STOCK = 25.0    # FIXME

# killkey.py material heightLargeSection
# cap height = 8mm or so

mat = init_material(sys.argv[1])
cap_height = float(sys.argv[2])

PIN_HEIGHT = 10.0
TOTAL_HEIGHT = cap_height + PIN_HEIGHT
BASE = TOTAL_HEIGHT - STOCK

D_CAP = 10.5
D_PIN = 5.5
D_INNER = 2.9

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

# trim the stock
g.move(z=0)
hole(g, mat, BASE, d=D_CAP, offset="outside", fill=True)

# cut the inner hole
g.move(z=BASE)
if mat['tool_size'] >= 3.0:
    drill(g, mat, -PIN_HEIGHT)
else:
    hole(g, mat, -PIN_HEIGHT, d=D_INNER, offset="inside", fill=True)

# cut the outer part
g.move(z=BASE)
hole(g, mat, -PIN_HEIGHT, d=D_CAP, di=D_PIN, fill=True)

# bottom part (thick)
g.move(z=BASE)
hole(g, mat, -TOTAL_HEIGHT, d=D_CAP, offset="outside", fill=False)

g.move(z=20.0)
g.spindle()
