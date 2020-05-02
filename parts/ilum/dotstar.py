from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole

mat = init_material("np883-aluminum-1.0")
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)

# DOTSTAR_Z = 52.0; // but check, to be sure. 1st dotstar center.
# and origin, since it is recessed in

DOTSTAR_PITCH = 7.0
depths = [-0.5, -1.0, -2.0]

for d in depths:
    for i in range(4):
        x = i * DOTSTAR_PITCH
        travel(g, mat, y=x)
        g.move(z=0)
        hole(g, mat, d, d=1.7)