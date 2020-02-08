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

mat = init_material(sys.argv[1])
print("material:", mat)
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None, print_lines=False)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()
g.move(z=0)

# cut the curve - expects a ball cutter
hill(g, mat, D_OUTER, X_CAPSULE, Y_CAPSULE, False)
tool_change(g, mat, 1)

#center bolt
travel(g, mat, x=BOLT)
hole(g, mat, bottom, d=4.4)
hole(g, mat, -1.5, d=8.0)

# power
travel(g, mat, x=PORT)
hole(g, mat, bottom, d=11.0)

# switch
BUTTON_HEAD_D = 7.2

travel(g, mat, x=SWITCH)    
hole(g, mat, bottom, d=BUTTON_HEAD_D + 0.2)
D = 8.0
SPACE = PORT - 11.0/2

travel(g, mat, x=SWITCH - BUTTON_HEAD_D/2)
rectangleTool(g, mat, -2.0, (X_CAPSULE - SPACE) - (SWITCH - BUTTON_HEAD_D/2), D, D/2, "left", "inner", True)

#### capsule cut #######
travel(g, mat, x=TRIM/2)
rectangleTool(g, mat, bottom, X_CAPSULE-TRIM, Y_CAPSULE-TRIM, 3.175/2, "left", "outer", False)

