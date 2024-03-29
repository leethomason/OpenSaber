import sys
import math
from utility import *
from material import init_material
from rectangleTool import rectangle
from mecode import G
from hole import hole2

DECO_DEPTH = -0.5
D0 = 31.60
DI_BRASS = 29.0 + 0.1
DO_BRASS = D0
EXTRA = 0.2

D_RING0 = 12.0    # center line
R_RING0 = D_RING0/2

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

D_HEAD = 6.8 + 0.2
D_NUT = 9.0
D_ROD = 3.6

A_MAKER = [-30, -10, 30, 150, 170, 210]
R_MAKER = 12.0

D_LED = 7.0
D_TOP = 12.0
D_TOP_CAP = 17.0

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
BIT = mat["tool_size"]

print("section", section)
print("flattenCut", flattenCut)
print("cut0", cut0)
print("cut1", cut1)
print("BIT", BIT)

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
comment(g, section)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

def flatten():
    travel(g, mat, x=0, y=0)
    g.move(z=0)
    hole2(g, mat, flattenCut, d=DO_BRASS + BIT*2, fill=True)

def bottom(arr):
    r = []
    for a in arr:
        r.append((180.0 - a) * math.pi / 180.0)
    return r

def top(arr):
    r = []
    for a in arr:
        r.append(a * math.pi / 180.0)
    return r

def holes(arr, r, depth, d):
    for a in arr:
        travel(g, mat, x=r * math.cos(a), y=r * math.sin(a))
        g.move(z=flattenCut)
        hole2(g, mat, depth, d=d)

def toFlat(x, y):
    travel(g, mat, x=x, y=y)
    g.move(z=flattenCut)

if section == "baseBottom":
    # cut0: should cut half way through (+eps)
    # cut1: unused

    flatten()
    holes(bottom(A_ANCHOR_ROD), R_ANCHOR_ROD, cut0, D_HEAD)
    holes(bottom(A_TUBE_ROD), R_TUBE_ROD, cut0, DI_TUBE_ROD)
    holes(bottom(A_BOLT_ROD), R_BOLT_ROD, cut0, D_HEAD)
    # doesn't need LED - cut from top
    # cut the alignment to the carriage:
    toFlat(0, -D0/2)
    rectangle(g, mat, cut0, 0, D0/2, False, "bottom")
    g.move(z=2)

elif section == "baseTop":
    # cut0: should cut half way through (+eps)
    # cut1: all the way through the stock

    flatten()
    holes(top(A_ANCHOR_ROD), R_ANCHOR_ROD, cut1, D_ROD)
    holes(top(A_TUBE_ROD), R_TUBE_ROD, cut0, DO_TUBE_ROD)
    holes(top(A_BOLT_ROD), R_BOLT_ROD, cut1, D_ROD)

    toFlat(0, 0,)
    hole2(g, mat, DECO_DEPTH, r=R_RING0 + BIT, fill=False)

    toFlat(0, 0)
    #hole2(g, mat, DECO_DEPTH, r=R_RING1 + BIT/2, fill=False)
    holes(top(A_MAKER), R_MAKER, DECO_DEPTH, BIT)

    toFlat(0, 0)
    hole2(g, mat, cut1, d=D_LED, fill=True)    

    toFlat(0, 0)
    hole2(g, mat, cut0, d=DI_BRASS + BIT*2, fill=False)
    hole2(g, mat, cut1, d=DO_BRASS + BIT*2, fill=False)

elif section == "top":
    # cut0: inset for tubes (~1.5mm)
    # cut1: all the way through the stock

    flatten()
    holes(bottom(A_ANCHOR_ROD), R_ANCHOR_ROD, cut1, D_ROD)
    holes(bottom(A_TUBE_ROD), R_TUBE_ROD, cut0, DO_TUBE_ROD)
    holes(bottom(A_TUBE_ROD), R_TUBE_ROD, cut1, DI_TUBE_ROD)
    holes(bottom(A_BOLT_ROD), R_BOLT_ROD, cut1, D_ROD)

    toFlat(0, 0)
    hole2(g, mat, cut1, d=D_TOP, fill=True)

    hole2(g, mat, cut1, d=DI_BRASS + BIT*2, fill=False)

elif section == "topCap":
    # cut0: inset for nut (~1.0mm)
    # cut1: all the way through the stock

    flatten()
    holes(bottom(A_ANCHOR_ROD), R_ANCHOR_ROD, cut1, D_NUT)
    holes(bottom(A_TUBE_ROD), R_TUBE_ROD, cut1, DI_TUBE_ROD)
    holes(bottom(A_BOLT_ROD), R_BOLT_ROD, cut1, D_ROD)
    holes(bottom(A_BOLT_ROD), R_BOLT_ROD, cut0, D_NUT)

    toFlat(0, 0)
    hole2(g, mat, cut1, d=D_TOP_CAP, fill=False)    

    hole2(g, mat, cut1, d=DO_BRASS + BIT*2, fill=False)

elif section == "repair":
    # cut0: should cut half way through (+eps)
    # cut1: unused

    holes(bottom(A_ANCHOR_ROD), R_ANCHOR_ROD, cut0, D_HEAD)
    holes(bottom(A_BOLT_ROD), R_BOLT_ROD, cut0, D_HEAD)
    g.move(z=2)

g.move(z=2)
travel(g, mat, x=0, y=0)
g.move(z=10)
spindle(g)






