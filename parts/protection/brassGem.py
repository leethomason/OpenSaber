import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from rectangle import rectangle
from mecode import G
from hole import hole

# positied so that bit is centered at top.

'''
ECHO: "Brass cut. From alignment hole. Tool center."
ECHO: "Length of section", 85.4
Brass switch cutout center d=", 19, "m=", 0
ECHO: "StartY w/bit", -25.9875
ECHO: "EndY w/Bit", 62.5875
ECHO: "P0", 0, 29.3875
ECHO: "P1", 6, 42.27
ECHO: "P2", 0, 55.1525
ECHO: "P3", -6, 42.27
'''

DZ_BRASS = 85.4
DEPTH = -6.0
END_DEPTH = -12.0

X0 = -6.0
X1 =  6.0
Y0 = 29.3875
Y1 = 55.1525
YM = (Y0 + Y1) / 2.0
CUTY0 = -25.9875
CUTY1 = 62.5875

mat = init_material(sys.argv[1])
top = int(sys.argv[2])

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

g.move(z=2)
g.move(y=Y0)

g.feed(mat['feed_rate'])
g.spindle('CW', mat['spindle_speed'])

g.move(z=0)

def path(g, total_plunge, plunge):
    g.move(x=X1, y=YM, z=total_plunge + plunge * 0.25)
    g.move(x=0, y=Y1, z=total_plunge + plunge * 0.50)
    g.move(x=X0, y=YM, z=total_plunge + plunge * 0.75)
    g.move(x=0, y=Y0, z=total_plunge + plunge)

steps = calc_steps(DEPTH, -mat['pass_depth'])
run_3_stages_abs(path, g, steps)

if top == 1:
    g.move(z=2)
    g.move(y=0)
    g.move(z=0)
    hole(g, mat, DEPTH, d=19.0, offset="inside", fill=False)
    g.move(z=0)
    rectangleTool(g, mat, DEPTH, 19.0 - mat['tool_size'], -CUTY0, 0, "top", "center")
else:
    g.move(z=2)
    g.move(x=0, y=0)
    g.move(z=0)
    hole(g, mat, DEPTH, d=4.2, offset="inside")


g.move(z=2)
g.move(y=CUTY0)
g.move(z=0)
rectangle(g, mat, END_DEPTH, 32.0, 0.0, 0.0, "bottom")

g.move(z=2)
g.move(y=CUTY1)
g.move(z=0)
rectangle(g, mat, END_DEPTH, 32.0, 0.0, 0.0, "bottom")

g.move(z=15)
g.spindle()

