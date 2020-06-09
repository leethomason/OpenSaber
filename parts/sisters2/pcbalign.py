from utility import *
from material import init_material
from rectangleTool import rectangleTool, overCut
from mecode import G
from hole import hole
from drill import drill

C = 25.4
D = 0.05 * C

DX = 1.5 * C
DY = 0.7 * C
NDRILL = 15

STOCK = 6.4
DZ_PART = 2.0
DZ_TROUGH = 0.8

Z_PART = -STOCK + DZ_PART
Z_TROUGH_BOTTOM = Z_PART - DZ_TROUGH
Z_BOTTOM = -STOCK
PAD = 0.5

DZ_PART_TO_BOTTOM = Z_PART - Z_BOTTOM
DZ_TROUGH_TO_BOTTOM = Z_TROUGH_BOTTOM - Z_BOTTOM

DY_TROUGH = 0.55 * C

TRIM = 0.15
HALF_TRIM = TRIM / 2

def calcY(y):
    return 0.6 * C - y

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


def part(g, mat, px, py, size):
    x = px
    y = calcY(py) - size/2 - HALF_TRIM    
    d = -DZ_PART_TO_BOTTOM - PAD

    g.move(z=0)

    addY = mat['tool_size']
    g.move(x=x, y=y-addY/2)
    g.move(z=Z_PART)
    rectangleTool(g, mat, d, size+TRIM, size+TRIM+addY, 0, "bottom", "inner", fill=True)

    '''
    # overcut
    x = px - size/2 - HALF_TRIM
    y = calcY(py) - size/2 - HALF_TRIM    
    g.move(x=x, y=y)
    g.move(z=Z_PART)
    overCut(g, mat, d, size + TRIM, size + TRIM)
    '''

    g.move(z=0)

mat = init_material("np883-hdpe-3.175")
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None, print_lines=False)
nomad_header(g, mat, CNC_TRAVEL_Z)

g.absolute()

# level part
g.move(x=-D + DX/2, y=-D)
g.move(z=0)
rectangleTool(g, mat, DZ_PART - STOCK, DX, DY, 0, "bottom", "outer", fill=True) # -6.0

tool_change(g, mat, "1")
mat = init_material("np883-hdpe-1.0")
tool = 1.0
half = 0.5

# accel
part(g, mat, 1.2904*C, 0.2708*C, 2.06)
# amp
part(g, mat, 0.1429*C, 0.3045*C, 3.10)

#drill
for x in range(NDRILL):
    g.move(z=(Z_PART))
    g.move(x=0.1 * C * x, y=0)
    g.move(z=Z_PART)
    drill(g, mat, -DZ_PART_TO_BOTTOM - PAD)
    g.move(z=(Z_PART))

for x in range(NDRILL):
    g.move(z=(Z_PART))
    g.move(x=0.1 * C * x, y=0.6 * C)
    g.move(z=Z_PART)
    drill(g, mat, -DZ_PART_TO_BOTTOM - PAD)
    g.move(z=(Z_PART))


tool_change(g, mat, "3")
mat = init_material("np883-hdpe-3.175")
tool = 3.175
half = tool/2

# cut trough
g.move(x=-D - half, y=-D + (DY / 2))
g.move(z=Z_PART)
rectangleTool(g, mat, -DZ_TROUGH, DX + tool, DY_TROUGH - tool, 0, "left", "center", fill=True)
g.move(z=0)

# cut board
g.move(z=0)
g.move(x=-D, y=-D + DY/2)
g.move(z=Z_PART)
rectangleTool(g, mat, -DZ_PART_TO_BOTTOM - PAD, DX, DY, 0, "left", "outer")

# clean up
g.move(z=10)
g.spindle()

