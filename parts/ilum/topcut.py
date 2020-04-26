from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G

mat = init_material("np883-aluminum-3.175")
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)

start = 51.0
end = 90.25
width = 16.5

# origin at bottom center.
g.move(y=start)
g.move(z=0)
rectangleTool(g, mat, -10, width, end - start, 0, "bottom", 'inner')
