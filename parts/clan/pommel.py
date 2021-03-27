import sys
from utility import *
from material import init_material
from rectangleTool import rectangleTool
from mecode import G
from hole import hole
from drill import drill

# pommel.py material depth

mat = init_material(sys.argv[1])
depth = float(sys.argv[2])

g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None)
nomad_header(g, mat, CNC_TRAVEL_Z)
g.absolute()

def doArc(g, r, angle0, angle1):
    N = 10

    def absPath(g, total_plunge, plunge):
        for a in range(angle0, angle1 + 1):
            x = math.cos(math.radians(a)) * r
            y = math.sin(math.radians(a)) * r
            d = 0.5 * plunge * (a - angle0) / (angle1 - angle0)
            g.move(x=x, y=y, z=total_plunge + d)

        for a in range(angle1, angle0, -1):
            x = math.cos(math.radians(a)) * r
            y = math.sin(math.radians(a)) * r
            d = 0.5 * plunge * (a - angle0) / (angle1 - angle0)
            g.move(x=x, y=y, z=total_plunge + d - plunge/2)
        
        x = math.cos(math.radians(angle0)) * r
        y = math.sin(math.radians(angle0)) * r
        g.move(x=x, y=y, z=total_plunge + plunge)

    x = math.cos(math.radians(angle0)) * r
    y = math.sin(math.radians(angle0)) * r
    g.move(z=3)
    g.move(x=x, y=y)
    g.move(z=0)

    steps = calc_steps(depth, -mat['pass_depth'])
    run_3_stages_abs(absPath, g, steps)

for i in range(0, 3):
    doArc(g, 10, i*120 + 0, i*120 + 70)
    doArc(g, 5, i*120 + 12, i*120 + 58)

g.move(z=15)
g.spindle()
