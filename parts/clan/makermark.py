import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole

# makermark.py material depth1 depth2 odd_depth stride "center"/"bottom"

mat = init_material(sys.argv[1])
depth1 = float(sys.argv[2])
depth2 = float(sys.argv[3])
odd_depth = float(sys.argv[4])
stride = float(sys.argv[5])
start = sys.argv[6]

origin_y = 0
if start == "bottom":
    origin_y = -stride * 1.5


