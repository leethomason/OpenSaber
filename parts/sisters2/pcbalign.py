from utility import *
from material import init_material
from rectangleTool import rectangleTool, overCut
from mecode import G
from hole import hole
from drill import drill

C = 25.4
D = 0.05 * C
PINY = 0.6 * C

DX = 2.2 * C
DY = 0.7 * C
NDRILL = 15

DZ_SLIDE = 2.0  # how far the part slides in - initial plane
DZ_CHIP = 1.6   # 1.0mm chip: 1.6mm below z, 2.6mm hole, 0.4mm edge
                # 0.8mm chip: 1.6mm below z, 2.4mm hole, 0.2mm edge
                # board to base clearance (for solder paste) 0.8 - 0.2 = 0.6
DZ_EDGE = DZ_CHIP + 0.1

STOCK = 6.5      # if we cut the outside
TRIM_PCB = 0.1   # each side
TRIM_CHIP = 0.5  # each side
BORDER = 3.0
HANDLE = 10.0    # section so part can be lifted

def calcY(y):
    return D + PINY - y

def calcX(x):
    return D + x

def line(g, mat, cut_depth, dx, dy):
    g.comment("line")
    with GContext(g):
        g.relative()

        g.move(x=-dx/2, y=-dy/2)
        def path(g, plunge, total_plunge):
            g.move(x=dx, y=dy, z=plunge/2)
            g.move(x=-dx, y=-dy, z=plunge/2)

        steps = calc_steps(cut_depth, -mat['pass_depth'])
        run_3_stages(path, g, steps)
        g.move(x=dx/2, y=dy/2)
        g.move(z=-cut_depth)


def part(g, mat, px, py, size, height):
    x = calcX(px)
    y = calcY(py)

    # 1.7 - 1.6 - 0.8 = -0.7 - 1.7 = 2.4mm hole
    # 1.7 - 1.6 - 1.0 = -0.9 - 1.7 = 2.6mm hole
    d = (DZ_EDGE - DZ_CHIP - height)

    g.move(z=0)
    g.move(x=x, y=y)
    g.move(z=-DZ_EDGE)
    rectangleTool(g, mat, d, size + TRIM_CHIP * 2, size + TRIM_CHIP * 2, 0, "center", "inner", fill=True)

    # overcut
    g.move(z=-DZ_EDGE)
    g.move(x=x - size/2 - TRIM_CHIP, y=y - size/2 - TRIM_CHIP)
    overCut(g, mat, d, size + TRIM_CHIP * 2, size + TRIM_CHIP * 2)

    g.move(z=0)

mat = init_material("np883-hdpe-3.175")
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None, print_lines=False)
nomad_header(g, mat, CNC_TRAVEL_Z)

g.absolute()

# level part
# really need overcut support. until then...hole the corners.
g.move(x=-D, y=-D)
g.move(z=0)
hole(g, mat, -DZ_EDGE, d=4.0)
g.move(x=-D + DX, y=-D)
g.move(z=0)
hole(g, mat, -DZ_EDGE, d=4.0)
g.move(x=-D+DX, y=-D+DY)
g.move(z=0)
hole(g, mat, -DZ_EDGE, d=4.0)
g.move(x=-D, y=-D+DY)
g.move(z=0)
hole(g, mat, -DZ_EDGE, d=4.0)

# first the edge
g.move(x=-D + DX/2, y=-D + DY/2)
g.move(z=0)
rectangleTool(g, mat, -DZ_EDGE, DX + TRIM_PCB*2, DY + TRIM_PCB*2, 0, "center", "inner", fill=True)

# tool change early, so that it gets leveled.
tool_change(g, mat, "1")
mat = init_material("np883-hdpe-1.0")
# accel
part(g, mat, 1.2904*C, 0.2708*C, 2.10, 1.0)
# amp
part(g, mat, 0.1429*C, 0.3045*C, 3.10, 0.8)

tool_change(g, mat, "3")
mat = init_material("np883-hdpe-3.175")

# then the deep
g.move(x=-D + DX/2, y=-D + DY/2)
g.move(z=-DZ_EDGE)
rectangleTool(g, mat, -(DZ_SLIDE - DZ_EDGE), DX - D*2, DY + TRIM_PCB*2, 0, "center", "inner", fill=True)

# and a section so it can be pulled out.
EXTRA = 0.2
g.move(z=0)
g.move(x=-D + DX/2, y=-D + DY/2)
rectangleTool(g, mat, -DZ_SLIDE, HANDLE, DY + BORDER * 2 + EXTRA, 0, "center", "inner", fill=True)

# cut board
g.move(z=0)
g.move(x=-D + DX/2, y=-D + DY/2)
rectangleTool(g, mat, -STOCK, DX + BORDER*2, DY + BORDER*2, 0, "center", "outer")

# clean up
g.move(z=10)
g.spindle()

