from cmath import cos, sin
from math import radians
import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from rectangleTool import rectangle
from mecode import G
from hole import hole
from drill import drill

STD_THICK = 3.0 # stock is about 3.3
STD_DEPTH = -STD_THICK - 0.2
DECO_DEPTH = -0.5
HALF_DEPTH = -1.2
D0 = 31.60
DI_BRASS = 29.0
EXTRA = 0.2
D_LED = 7.0

RING0 = 10.0 + 1.6
RING1 = 17.0 + 1.6
RING2 = 24.0 + 1.6

# ANCHOR_ROD: threaded, holds the crystal in place
A_ANCHOR_ROD = radians(0)
R_ANCHOR_ROD = 9.0

# TUBE_ROD: hollow tube, holds the piece apart, runs wires through
A_TUBE_ROD = radians(60)
DO_TUBE_ROD = 4.7
R_TUBE_ROD = 12.0

# BOLT_ROD: threaded, holds the piece together
A_BOLT_ROD = radians(120)
R_BOLT_ROD = 11.0

D_HEAD = 6.8
D_NUT = 9.0
D_ROD = 3.6

# chamber material section stockThickness
# section = bottomCap, base, top, topCap

mat = init_material(sys.argv[1])
section = sys.argv[2]
stockThickness = float(sys.argv[3])

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

depthPart = STD_THICK - stockThickness

# flatten out the top
travel(g, mat, x=0, y=0)
g.move(z=0)
hole(g, mat, depthPart, d=D0, offset="inside", fill=True)

dCenter = D_LED # bottomCap, bottom
if section == "top":
    dCenter = 12.0
elif section == "topCap":
    dCenter = 16.0

if section == "bottomCap" or section == "base":
    travel(g, mat, x=0, y=0)
    g.move(z=depthPart)
    hole(g, mat, STD_DEPTH, d=D_LED, offset="inside", fill=True)
    g.move(z=2)

# Rod (tension) to hold the crystal in place
dCrystal = D_HEAD # bottomCap
if section == "bottom" or section == "top":
    dCrystal = D_ROD
elif section == "topCap":
    dCrystal = D_NUT
    
for r in [0, radians(180)]:
    travel(g, mat, 
        x=R_ANCHOR_ROD * cos(A_ANCHOR_ROD + r), 
        y=R_ANCHOR_ROD * sin(A_ANCHOR_ROD + r))
    g.move(z=depthPart)
    hole(g, mat, STD_DEPTH, d=dCrystal, offset="inside", fill=True)
    g.move(z=2)

# Tubes (compression) to hold the piece apart and run wires
dTube = D_ROD # bottomCap
if section == "bottom" or section == "top":
    dTube = DO_TUBE_ROD
elif section == "topCap":
    dTube = D_ROD

for r in [0, radians(180)]:
    travel(g, mat, 
        x=R_TUBE_ROD * cos(A_TUBE_ROD + r), 
        y=R_TUBE_ROD * sin(A_TUBE_ROD + r))
    g.move(z=depthPart)
    hole(g, mat, STD_DEPTH, d=dTube, offset="inside", fill=True)
    g.move(z=2)

# Threaded rod to secure outer ring (tension) with tube coverings
dBolt = D_HEAD # bottomCap
if section != "bottomCap":
    dBolt = D_ROD

for r in [0, radians(180)]:
    travel(g, mat, 
        x=R_BOLT_ROD * cos(A_BOLT_ROD + r), 
        y=R_BOLT_ROD * sin(A_BOLT_ROD + r))
    g.move(z=depthPart)
    hole(g, mat, STD_DEPTH, d=dBolt, offset="inside", fill=True)
    g.move(z=2)

# special case: partially sink on the topCap
if section == "topCap":
    for r in [0, radians(180)]:
        travel(g, mat, 
            x=R_BOLT_ROD * cos(A_BOLT_ROD + r), 
            y=R_BOLT_ROD * sin(A_BOLT_ROD + r))
        g.move(z=depthPart)
        hole(g, mat, HALF_DEPTH, d=D_NUT, offset="inside", fill=True)
        g.move(z=2)

# lock to the main carriage
if section == "bottomCap":
    travel(g, mat, x=0, y=-D0/2)
    g.move(z=depthPart)
    rectangle(g, mat, STD_DEPTH, 0, D0/2, False, "bottom")
    g.move(z=2)

# Bottom decoration (fancy!)
if section == "bottom":
    # Bottom rings:
    for d in [RING0, RING1]:
        travel(g, mat, x=0, y=0)
        g.move(z=depthPart)
        hole(g, mat, DECO_DEPTH, d=d, offset="middle", fill=False)
        g.move(z=2)

    # Maker's mark
    for r in [-30, -30 + 20, -30 + 60, 150, 150 + 20, 150 + 60]:
        travel(g, mat, 
            x=RING2/2 * cos(radians(r)), 
            y=RING2/2 * sin(radians(r)))
        g.move(z=depthPart)
        drill(g, mat, DECO_DEPTH)
        g.move(z=2)


# The outer cut!
dOuter = D0
if section == "top" or section == "bottom":
    outer = DI_BRASS

travel(g, mat, x=0, y=0)
g.move(z=0)
hole(g, mat, -stockThickness - EXTRA, d=dOuter, offset="outside", fill=False)
g.move(z=10)

