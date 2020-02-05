from utility import *
from material import init_material
from hole import hole
from mecode import G

# inner line to inner line: 47mm
# 2.5mm space
# capsule at center

mat = init_material("np883-aluminum-1.0")
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)

CY = 22.2 + 42.0 / 2.0
PITCH = 7
DEPTH = -6.0

g.spindle()
travel(g, mat, y = CY - 1.5 * PITCH)
g.dwell(1)
travel(g, mat, y = CY - 0.5 * PITCH)
g.dwell(1)
travel(g, mat, y = CY + 0.5 * PITCH)
g.dwell(1)
travel(g, mat, y = CY + 1.5 * PITCH)
g.dwell(1)

travel(g, mat, y = CY - 1.5 * PITCH)
hole(g, mat, DEPTH, d=1.6)
travel(g, mat, y = CY - 0.5 * PITCH)
hole(g, mat, DEPTH, d=1.6)
travel(g, mat, y = CY + 0.5 * PITCH)
hole(g, mat, DEPTH, d=1.6)
travel(g, mat, y = CY + 1.5 * PITCH)
hole(g, mat, DEPTH, d=1.6)
