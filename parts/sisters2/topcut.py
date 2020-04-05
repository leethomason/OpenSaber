from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole

# positioned so that body is in the y axis
# origin is at the emitter end

DEPTH = -6.0

M_END = 177.5
M_BOLT = 130.5
M_POWER = 142.0
M_SWITCH = 157.0

D_SWITCH = 12.0
D_DISPLAY = 8.3  
M_DISPAY0 = 93.0
M_DISPAY1 = 119.0
D_BOLT = 8.0

mat = init_material(argv[1])
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)

# switch
travel(g, mat, y = M_END - M_SWITCH - D_SWITCH/2)
rectangleTool(g, mat, DEPTH,
    D_SWITCH, M_SWITCH - M_POWER + D_SWITCH, 
    D_SWITCH /2,
    "bottom", "inner", fill=False, adjust_trim=True)

# plate for screw head
travel(g, mat, y = M_END - M_BOLT)
hole(g, mat, -1.0, d=D_BOLT)        # fixme test
hole(g, mat, DEPTH, d=4.2)

# display
travel(g, mat, y = M_END - M_DISPAY1)
rectangleTool(g, mat, DEPTH, 
    D_DISPLAY, M_DISPAY1 - M_DISPAY0, 
    D_DISPLAY /2,
    "bottom", "inner", fill=False, adjust_trim=True)

