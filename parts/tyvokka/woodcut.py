from utility import *
from material import init_material
from capsule import capsule
from mecode import G
from plane import plane
from covertec import hill
from hole import hole

# Top of stock.
H_WOOD = 8.0
H_STOCK = 14.60
X_CAPSULE = 42.0
Y_CAPSULE = 16.2
Z_PAD = 0.5
TOOL = 3.175
DZ_CENTER = (X_CAPSULE - Y_CAPSULE) / 2
MAT = 'np883-hardwood-{0}'.format(TOOL)
D_OUTER = 32.0  # FIXME


center = Rectangle(0, -Y_CAPSULE/2, X_CAPSULE, Y_CAPSULE/2)
bounds = Bounds(TOOL, center)
bottom = -H_WOOD - Z_PAD
top = -(H_STOCK - H_WOOD)

mat = init_material(MAT)
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None, print_lines=False)
nomad_header(g, mat, H_STOCK + CNC_TRAVEL_Z)

g.absolute()
g.move(z=0)

if (H_STOCK < H_WOOD):
    raise RuntimeError("Stock can be the same as the wood, but not lower.")

if H_STOCK > H_WOOD:
    plane(g, mat, top, bounds.outer.x0, bounds.outer.y0, bounds.outer.x1, bounds.outer.y1)

g.move(z=top)

# cut the curve
hill(g, mat, D_OUTER, bounds.center.dx, bounds.center.dy)

#center bolt
HEAD = 7.0
HEAD_H = 4.0    # was 4.5 - go a little proud
BOLT = 4.3

travel(g, mat, x=bounds.cx)
hole(g, mat, bottom, d=BOLT)
hole(g, mat, -HEAD_H, d=HEAD)

# power
POWER_D = 8.0
POWER_OUTER_D = 11.0
POWER_OUTER_H = 2.0

travel(g, mat, x=bounds.cx - DZ_CENTER)
hole(g, mat, bottom, d=POWER_D)
hole(g, mat, -POWER_OUTER_H, d=POWER_OUTER_D)

# switch
SWITCH_D = 4.0 # 3.6 # 3.5
SWITCH_INSET_D = 8.0
SWITCH_INSET_DEPTH = -2.0

travel(g, mat, x=bounds.cx + DZ_CENTER)    
hole(g, mat, bottom, d=SWITCH_D)
hole(g, mat, SWITCH_INSET_DEPTH, d=SWITCH_INSET_D)

#### capsule cut #######
travel(g, mat, x=bounds.cx)
capsule(g, mat, bottom, bounds.center.dx, bounds.center.dy, "outside", True, 'x', Z_PAD + 1.0)
