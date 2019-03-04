from utility import *
from material import init_material
from capsule import capsule
from mecode import G
from plane import plane
from covertec import hill
from hole import hole

# Start at top of wood, NOT top of stock.
# This can make centering the machine a little tricky.
H_WOOD = 8.0
H_STOCK = 19.3
Z_PAD = 0.5
TOOL_R = 3.175 / 2
SMOOTH_PATHS = False
MAT = 'np883-pine-3.175'


Z_STOCK = H_STOCK - H_WOOD
DELTA = 3
SMALL = 0.5

X_CAPSULE_NOM = 42.0
D_CAPSULE = 16.2
DZ_CENTER = (X_CAPSULE_NOM - D_CAPSULE) / 2

R = D_CAPSULE / 2
D_OUTER = 37.9

TRAVEL = H_STOCK + 3

# origin at left end of piece.
mat = init_material(MAT)
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None, print_lines=False)
nomad_header(g, mat, TRAVEL)

g.absolute()
g.move(z=TRAVEL)

g.spindle('CW', mat['spindle_speed'])
g.feed(mat['travel_feed'])

# flatten and take off the top of stock
if Z_STOCK > 0:
    g.move(x=-DELTA, y=-D_CAPSULE/2-DELTA)
    g.move(z=Z_STOCK)
    g.feed(mat['feed_rate'])
    plane(g, mat, H_WOOD - H_STOCK, X_CAPSULE_NOM + DELTA * 2, D_CAPSULE + DELTA * 2)

# cut the curve
g.move(z=CNC_TRAVEL_Z)
g.spindle()
g.move(x=-DELTA, y=-SMALL)
g.move(z=Z_STOCK)
g.move(z=0)

hill(g, mat, D_OUTER, X_CAPSULE_NOM + DELTA*2, D_CAPSULE+SMALL*2)
g.move(z=CNC_TRAVEL_Z)

# insets
g.feed(mat['travel_feed'])

#center bolt
HEAD = 7.0
HEAD_H = 4.5
BOLT = 4.3
g.move(x=X_CAPSULE_NOM/2, y=0)

g.move(z=CNC_TRAVEL_Z)

g.feed(mat['feed_rate'])
hole(g, mat, -HEAD_H, HEAD/2)
hole(g, mat, -H_WOOD - Z_PAD, BOLT/2)

# power
POWER_D = 8.0
POWER_OUTER_D = 11.0
POWER_OUTER_DEPTH = -1.0
POWER_X = X_CAPSULE_NOM/2 - DZ_CENTER

g.move(z=CNC_TRAVEL_Z)
g.move(x=POWER_X, y=0)
hole(g, mat, -H_WOOD - Z_PAD, POWER_D/2)

# switch
SWITCH_D = 4.0 # 3.6 # 3.5
SWITCH_X = X_CAPSULE_NOM/2 + DZ_CENTER
SWITCH_INSET_D = 8.0
SWITCH_INSET_DEPTH = -1.0

g.move(z=CNC_TRAVEL_Z)
g.move(x=SWITCH_X, y=0)
hole(g, mat, -H_WOOD - Z_PAD, SWITCH_D/2)

if SMOOTH_PATHS:
    # rounded inset 
    hole(g, mat, SWITCH_INSET_DEPTH, SWITCH_INSET_D/2 - TOOL_R)
else:
    hole(g, mat, SWITCH_INSET_DEPTH, SWITCH_INSET_D/2)

if SMOOTH_PATHS:
    #### rounded tool ######
    tool_change(g, 2)

    g.move(z=CNC_TRAVEL_Z)
    g.move(x=POWER_X, y=0)
    # rounded inset
    hole(g, mat, POWER_OUTER_DEPTH, POWER_OUTER_D/2)

    g.move(z=CNC_TRAVEL_Z)
    g.move(x=SWITCH_X, y=0)
    hole(g, mat, -H_WOOD - Z_PAD, SWITCH_D/2)
    # rounded inset 
    hole(g, mat, SWITCH_INSET_DEPTH, SWITCH_INSET_D/2)

    tool_change(g, 1)

#### capsule cut #######
g.move(x=X_CAPSULE_NOM/2, y=0)
g.move(z=0)

capsule(g, mat, -H_WOOD - Z_PAD, 
        X_CAPSULE_NOM, D_CAPSULE,
        "outside", True, 'x')
