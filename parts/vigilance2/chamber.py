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

DECO_DEPTH = -0.5
D0 = 31.60
DI_BRASS = 29.0
DO_BRASS = D0
EXTRA = 0.2

RING0 = 10.0 + 1.6
RING1 = 17.0 + 1.6
RING2 = 24.0 + 1.6

# ANCHOR_ROD: threaded, holds the crystal in place
A_ANCHOR_ROD = [0, 180]
R_ANCHOR_ROD = 9.0

# TUBE_ROD: hollow tube, holds the piece apart, runs wires through
A_TUBE_ROD = [60, 240]
DO_TUBE_ROD = 4.7
DI_TUBE_ROD = 3.8
R_TUBE_ROD = 12.0

# BOLT_ROD: threaded, holds the piece together
A_BOLT_ROD = [120, 300]
R_BOLT_ROD = 11.0

D_HEAD = 6.8
D_NUT = 9.0
D_ROD = 3.6

A_MAKER = [-30, -10, 30, 150, 170, 210]
R_MAKER = 12.0

BIT = 3.175
RING0 = 10.0 + BIT/2
RING1 = 17.0 + BIT/2

D_LED = 7.0

# chamber material section flatten cut0 cut1
# section = baseBottom, baseTop, top, topCap

# Stages:
# 1. flatten top
# 2. holes (drill/hole)
# 3. rings (hole)

print("chamber mat section('baseBottom', 'baseTop', 'top', 'topCap') flatten cut0 cut1")

mat = init_material(sys.argv[1])
section = sys.argv[2]
flattenCut = float(sys.argv[3])
cut0 = float(sys.argv[4])
cut1 = float(sys.argv[5])

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

def flatten():
    travel(g, mat, x=0, y=0)
    g.move(z=0)
    hole(g, mat, flattenCut, d=DO_BRASS, offset="outside", fill=True)

def bottom(arr):
    r = []
    for a in arr:
        r.append(radians(180 - a))
    return r

def top(arr):
    r = []
    for a in arr:
        r.append(radians(a))
    return r

def holes(arr, r, depth, d):
    for a in arr:
        travel(g, mat, x=r * cos(a), y=r * sin(a))
        g.move(z=flattenCut)
        if -d > 3.2:
            hole(g, mat, depth, d=d, offset="inside", fill=True)
        else:
            drill(g, mat, depth)
        g.move(z=2)

def ring(d, depth):
    travel(g, mat, x=0, y=0)
    g.move(z=flattenCut)
    hole(g, mat, depth, d=d, offset="middle", fill=False)
    g.move(z=2)

if section == "baseBottom":
    flatten()
    holes(bottom(A_ANCHOR_ROD), R_ANCHOR_ROD, cut0, D_HEAD)
    holes(bottom(A_TUBE_ROD), R_TUBE_ROD, cut0, DI_TUBE_ROD)
    holes(bottom(A_BOLT_ROD), R_BOLT_ROD, cut0, D_HEAD)
    # doesn't need LED - cut from top
    # cut the alignment to the carriage:
    travel(g, mat, x=0, y=-D0/2)
    g.move(z=flattenCut)
    rectangle(g, mat, cut0, 0, D0/2, False, "bottom")
    g.move(z=2)

elif section == "baseTop":
    flatten()
    holes(top(A_ANCHOR_ROD), R_ANCHOR_ROD, cut0, D_ROD)
    holes(top(A_TUBE_ROD), R_TUBE_ROD, cut0, DO_TUBE_ROD)
    holes(top(A_BOLT_ROD), R_BOLT_ROD, cut0, D_ROD)

    ring(RING0, DECO_DEPTH)
    ring(RING1, DECO_DEPTH)
    holes(top(A_MAKER), R_MAKER, DECO_DEPTH, 3.175)

    travel(g, mat, x=0, y=0)
    g.move(z=flattenCut)
    hole(g, mat, cut1, d=D_LED, offset="outside", fill=True)    

    ring(DI_BRASS + BIT/2, cut0)
    ring(DO_BRASS + BIT/2, cut1)

travel(g, mat, x=0, y=0)
g.move(z=10)





