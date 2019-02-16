from utility import *
from material import init_material
from capsule import capsule
from mecode import G
from plane import plane
from covertec import hill
from hole import hole

H_WOOD = 8.0
H_STOCK = 19.5
Z_PAD = 0.5

Z_STOCK = H_STOCK - H_WOOD
DELTA = 3
SMALL = 0.5

L_CAPSULE = 42.5
D = 16.2
R = D / 2
D_OUTER = 37.9

TRAVEL = H_STOCK + 3

# origin at left end of piece.
mat = init_material("np883-pine-3.175")
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None, print_lines=False)
nomad_header(g)

g.absolute()
g.move(z=TRAVEL)

g.spindle('CW', mat['spindle_speed'])
g.feed(mat['travel_feed'])

# flatten and take off the top of stock
g.move(x=-DELTA, y=-D/2-DELTA)
g.move(z=Z_STOCK)
g.feed(mat['feed_rate'])
plane(g, mat, H_WOOD - H_STOCK, L_CAPSULE + DELTA * 2, D + DELTA * 2)

# cut the curve
g.move(z=CNC_TRAVEL_Z)
g.spindle()
tool_change(g, '2')

g.move(x=-DELTA, y=-SMALL)
g.move(z=Z_STOCK)
g.move(z=0)

hill(g, mat, D_OUTER, L_CAPSULE + DELTA*2, D+SMALL*2)
g.move(z=CNC_TRAVEL_Z)

# insets
g.feed(mat['travel_feed'])
tool_change(g, '1')

#center bolt
HEAD = 7.0
HEAD_H = 4.5
BOLT = 4.3
g.move(x=L_CAPSULE/2, y=0)

g.move(z=CNC_TRAVEL_Z)

g.feed(mat['feed_rate'])
hole(g, mat, -HEAD_H, HEAD/2)
hole(g, mat, -H_WOOD - Z_PAD, BOLT/2)

# power
POWER_D = 8

g.move(z=CNC_TRAVEL_Z)
g.move(x=R, y=0)
hole(g, mat, -H_WOOD - Z_PAD, POWER_D/2)

# capsule
g.move(x=L_CAPSULE/2, y=0)
g.move(z=0)

tool = mat['tool_size']
ht = tool / 2
#print("d+tool {0}".format(D + tool))
#fixme: fix how capsule accounts for tool size
capsule(g, mat, -H_WOOD - Z_PAD, L_CAPSULE + tool*2, D + tool*2, None, True, 'x')
