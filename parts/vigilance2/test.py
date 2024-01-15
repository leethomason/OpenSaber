from cmath import cos, sin
import sys
import math
from utility import *
from material import init_material
from rectangleTool import rectangle
from mecode import G
from hole import hole2

print("test mat section('baseBottom', 'baseTop', 'top', 'topCap') flatten cut0 cut1")

D0 = 31.60
DI_BRASS = 29.0
DO_BRASS = D0
EXTRA = 0.2

mat = init_material(sys.argv[1])
section = sys.argv[2]
flattenCut = float(sys.argv[3])
cut0 = float(sys.argv[4])
cut1 = float(sys.argv[5])
BIT = mat["tool_size"]

print("flattenCut", flattenCut)
print("cut0", cut0)
print("cut1", cut1)
print("BIT", BIT)

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
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
        g.move(z=2)

def toFlat(x, y):
    travel(g, mat, x=x, y=y)
    g.move(z=flattenCut)

flatten()
toFlat(0, 0)
hole2(g, mat, -1.0, d=5.0, fill=False)    
hole2(g, mat, -2.0, d=4.0, fill=False)    



