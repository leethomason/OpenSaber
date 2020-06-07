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
NDRILL = 14
STOCK = 8.0
H = 2.0
H_TROUGH = 0.8
DY_TROUGH = 0.55 * C

ACCEL_X = 1.2904 * C
ACCEL_Y = 0.2708 * C
TRIM = 0.1
HALF_TRIM = TRIM / 2
ACCEL_SIZE = 2.0

def calcY(y):
    return -D + DY - y

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


mat = init_material("np883-hdpe-3.175")
g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None, print_lines=False)
nomad_header(g, mat, CNC_TRAVEL_Z)

g.absolute()

# level part
g.move(x=-D + DX/2, y=-D)
g.move(z=0)
rectangleTool(g, mat, H - STOCK, DX, DY, 0, "bottom", "outer", fill=True) # -6.0

tool_change(g, mat, 1)
mat = init_material("np883-hdpe-1.0")

tool = 1.0
half = 0.5

# cut trough
g.move(x=-D - half, y=-D + (DY / 2))
g.move(z=H - STOCK)
rectangleTool(g, mat, -H_TROUGH, DX + tool, DY_TROUGH - tool, 0, "left", "center")
g.move(z=0)

# accel
x = ACCEL_X - ACCEL_SIZE/2 - HALF_TRIM
y = calcY(ACCEL_Y) - ACCEL_SIZE/2 - HALF_TRIM
g.move(x=x, y=y)
g.move(z=(H - STOCK) - H_TROUGH)
d = -H_TROUGH - 0.2
overCut(g, mat, d, ACCEL_SIZE + TRIM, ACCEL_SIZE + TRIM)

x = ACCEL_X
y = calcY(ACCEL_Y)
g.move(x=x, y=y)
g.move(z=(H - STOCK) - H_TROUGH)
line(g, mat, d, 0, ACCEL_SIZE * 2)
g.move(z=0)

#drill
for x in range(NDRILL):
    g.move(z=(H - STOCK))
    g.move(x=0.1 * C * x, y=0)
    g.move(z=H - STOCK)
    drill(g, mat, -H - 0.2)
    g.move(z=(H - STOCK))

for x in range(NDRILL):
    g.move(z=(H - STOCK))
    g.move(x=0.1 * C * x, y=0.6 * C)
    g.move(z=H - STOCK)
    drill(g, mat, -H - 0.2)
    g.move(z=(H - STOCK))

g.move(z=0)
g.move(x=-D, y=-D + DY/2)
g.move(z=H - STOCK)
rectangleTool(g, mat, -H - 0.2, DX, DY, 0, "left", "outer")

# clean up
g.move(z=5)
g.spindle()

