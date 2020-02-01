from utility import *
from material import init_material
from drill import drill
from mecode import G

# inner line to inner line: 47mm
# 2.5mm space
# capsule at center

mat = init_material("np883-aluminum-3.175")
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)

CY = 42.0 / 2.0
PITCH = 7

travel(g, mat, y = CY - 1.5 * PITCH)
drill(g, mat, -1.0)
travel(g, mat, y = CY - 0.5 * PITCH)
drill(g, mat, -1.0)
travel(g, mat, y = CY + 0.5 * PITCH)
drill(g, mat, -1.0)
travel(g, mat, y = CY + 1.5 * PITCH)
drill(g, mat, -1.0)