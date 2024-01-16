import sys
from mecode import G
from material import init_material
from utility import comment, nomad_header, CNC_TRAVEL_Z, spindle
from rectangleTool import rectangle

# trim the origin point
# start on the edge of the lower left corner

print("origin.py mat depth")

mat = init_material(sys.argv[1])
depth = float(sys.argv[2])

LEN = 8.0

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
comment(g, "origin cut")
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

rectangle(g, mat, depth, 0, LEN, False, "bottom")
rectangle(g, mat, depth, LEN, 0, False, "left")

g.move(z = 5)
spindle(g)