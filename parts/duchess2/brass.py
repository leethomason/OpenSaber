import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import *
import math

#DEPTH = -2.0

D_VENT_INNER = 25.8
D_TUBE = 4.4
D_ROD_LOOSE = 3.6
D_ROD_NUT = 7.2
D_CRYSTAL_SPACE = 12.0

A_TILT = -5
A_BOLT_0 = 90 + A_TILT
A_BOLT_1 = 270 + A_TILT
ANGLE_OFFSET = 33

A_CHAMBER_CUT_0 = -15 + A_TILT
A_CHAMBER_CUT_1 = 15 + A_TILT
A_CHAMBER_CUT_2 = 45 + A_TILT
R_CHAMBER_CUT = 9.5
D_CHAMBER_CUT = 3.6

mat = init_material(sys.argv[1])
DEPTH = float(sys.argv[2])

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

def do_drill(angle, part_r, hole_r):
    travel(g, mat, x=math.cos(math.radians(angle)) * part_r, y=math.sin(math.radians(angle)) * part_r)
    g.move(z=0)
    hole_or_drill(g, mat, DEPTH, hole_r)

    travel(g, mat, x=math.cos(math.radians(angle + 180)) * part_r, y=math.sin(math.radians(angle + 180)) * part_r)
    g.move(z=0)
    hole_or_drill(g, mat, DEPTH, hole_r)

# start at the center.

# The TUBE is the y axis, which makes this math a little awkward.
travel(g, mat, x=D_VENT_INNER/2 - D_TUBE)
g.move(z=0)
rectangleTool(g, mat, DEPTH, 8, D_TUBE, D_TUBE/2, "left", "inner", False, True)
travel(g, mat, x=-(D_VENT_INNER/2 - D_TUBE))
g.move(z=0)
rectangleTool(g, mat, DEPTH, 8, D_TUBE, D_TUBE/2, "right", "inner", False, True)

# Now the rod
do_drill(-ANGLE_OFFSET, D_VENT_INNER/2 - D_ROD_NUT/2, D_ROD_LOOSE/2)

# spacers
do_drill(A_CHAMBER_CUT_0 + 90 - ANGLE_OFFSET, R_CHAMBER_CUT, D_CHAMBER_CUT/2)
do_drill(A_CHAMBER_CUT_1 + 90 - ANGLE_OFFSET, R_CHAMBER_CUT, D_CHAMBER_CUT/2)
do_drill(A_CHAMBER_CUT_2 + 90 - ANGLE_OFFSET, R_CHAMBER_CUT, D_CHAMBER_CUT/2)

# center and outside
travel(g, mat, x=0, y=0)
g.move(z=0)
hole(g, mat, DEPTH, d=D_CRYSTAL_SPACE, fill=False, offset="inside")
g.move(z=0)
hole(g, mat, DEPTH, d=D_VENT_INNER, fill=False, offset="outside")

