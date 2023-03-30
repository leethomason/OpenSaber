from mecode import G
import math
import argparse
from utility import nomad_header, CNC_TRAVEL_Z, travel
from hole import hole
from drill import drill
from material import init_material

TRIM = 0.5

def ring(inner, outer, height, stock, mat, g, maker):

    if height > stock:
        raise RuntimeError("Height is greater than stock.")

    drop = height - stock

    # flatten
    if drop < 0:
        travel(g, mat, x=0, y=0)
        g.move(z=0)
        hole(g, mat, drop, d=outer, offset="outside", fill=True)

    #ring
    if inner > 0:
        g.move(z=drop)
        hole(g, mat, -height - TRIM, d=inner, offset="inside", fill=False)

    g.move(z=drop)
    hole(g, mat, -height - TRIM, d=outer, offset="outside", fill=False)

    if maker:
        g.move(z=0)
        STEP = 20
        RAD = [math.radians(0), math.radians(STEP), math.radians(STEP*3)]
        R = (inner + outer) / 4
        for r in RAD:
            g.move(x=R * math.cos(r), y=R * math.sin(r))
            g.move(z=drop)
            drill(g, mat,-0.5)

    g.move(z=15)

def main():
    parser = argparse.ArgumentParser(description='Cut a hole in a cylinder. A "washer" of various dimensions. Head needs to be top of stock at (0, 0, 0). Will cut to 0.5mm below stock.')
    parser.add_argument('inner', help='Inner diameter.', type=float)
    parser.add_argument('outer', help='Outer diameter.', type=float)
    parser.add_argument('height', help='Height of the part.', type=float)
    parser.add_argument('stock', help='Height of the stock. (z=0 at top of stock)', type=float)
    parser.add_argument('material', help='The material to cut in standard machine-material-size format.', type=str)
    parser.add_argument('-m', '--maker', help="Add a maker's mark.", type=bool, default=False)
    args = parser.parse_args()

    mat = init_material(args.material)
    g = G(outfile='path.nc', aerotech_include=False, header=None, footer=None, print_lines=False)

    nomad_header(g, mat, CNC_TRAVEL_Z)
    g.spindle('CW', mat['spindle_speed'])
    g.move(z=0)
    ring(args.inner, args.outer, args.height, args.stock, mat, g, args.maker)
    g.spindle()

if __name__ == "__main__":
    main()
