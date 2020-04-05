import sys
from utility import *
from material import init_material
from mecode import G
from hill import hill
from hole import hole
from rectangleTool import rectangleTool

H_WOOD = 25.4 / 4
X_CAPSULE = 42.0
Y_CAPSULE = 18.0

BOLT = 21.0
PORT = BOLT + 12.0
SWITCH = BOLT - 12.0
TRIM = 0.2

bottom = -H_WOOD - 1.0

mat = init_material(sys.argv[1])
print("material:", mat)
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None, print_lines=False)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()
g.move(z=0)

#center bolt
travel(g, mat, x=BOLT)
hole(g, mat, bottom, d=4.4)
hole(g, mat, -1.5, d=8.0)

# power
travel(g, mat, x=PORT)
hole(g, mat, bottom, d=11.0)

# switch
BUTTON_HEAD_D = 8.0
travel(g, mat, x=SWITCH)    
hole(g, mat, bottom, d=BUTTON_HEAD_D)

D = 11.0
travel(g, mat, x=-BUTTON_HEAD_D)
rectangleTool(g, mat, -1.5, (SWITCH + BUTTON_HEAD_D/2 + 1.0) + BUTTON_HEAD_D, D, D/2, "left", "inner", True, adjust_trim=True)

#### capsule cut #######
travel(g, mat, x=TRIM/2)
rectangleTool(g, mat, bottom, X_CAPSULE-TRIM, Y_CAPSULE-TRIM, 3.175/2, "left", "outer", False, adjust_trim=True)

