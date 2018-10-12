from hole import hole
from hole import hole_abs
from mecode import G
from material import init_material
from utility import *
import math
import sys

mat = init_material(sys.argv[1])
tool_size = mat['tool_size']
outer_d = 29.0 + tool_size
cut_depth = -2.0
inner_d = 22.0 + tool_size

outer_r = outer_d / 2
inner_r = inner_d / 2

origin = 30
thetas = [origin - 30, origin + 30, origin + 90, origin + 150, origin + 210, origin + 270]
r_rod = 3.6 / 2
r_tube = 6.0 / 2

rods = [
    [-7, -7, r_rod],
    [ 7, -7, r_rod],
    [ 0,  10, r_rod],
    [ 0,  0, r_tube]
]


g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None, print_lines=False)

def x_r(theta, r):
    return math.cos(math.radians(theta)) * r

def y_r(theta, r):
    return math.sin(math.radians(theta)) * r

def g_arc(g, theta, r, direction, z=None):
    x = x_r(theta, r)
    y = y_r(theta, r)

    i = -g.current_position['x']
    j = -g.current_position['y']

    if z is not None:
        g.arc2(x=x, y=y, i=i, j=j, direction=direction, helix_dim='z', helix_len=z)
    else:
        g.arc2(x=x, y=y, i=i, j=j, direction=direction)
    pass

def g_move(g, theta, r, z=None):
    if z is None:
        g.abs_move(x=x_r(theta, r), y=y_r(theta, r))
    else:
        g.abs_move(x=x_r(theta, r), y=y_r(theta, r), z=z)

def path(g, z, dz):
    # start at thetas[0], outer_r
    for i in range(0, len(thetas), 2):
        g_move(g, thetas[i+0], outer_r, z)
        g_move(g, thetas[i+0], inner_r, z)
        g_arc(g, thetas[i+1], inner_r, 'CCW', z + i * dz / len(thetas))
        g_move(g, thetas[i+1], outer_r)
        g_arc(g, thetas[(i+2)%len(thetas)], outer_r, 'CCW', z + dz * (i + 1) / len(thetas))

g.feed(mat['feed_rate'])
g.absolute()

g.abs_move(z=CNC_TRAVEL_Z)
g_move(g, thetas[0], outer_r)
g.spindle('CW', mat['spindle_speed'])
g.abs_move(z=0)

steps = calc_steps(cut_depth, -mat['pass_depth'])
run_3_stages_abs(path, g, steps)

for r in rods:
    hole_abs(g, mat, cut_depth, r[2], r[0], r[1])

g.move(z=CNC_TRAVEL_Z)
g.spindle()

