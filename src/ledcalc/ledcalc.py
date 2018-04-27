# V = IR
# (Vb - Vf) = IR
# P = V * I

import argparse

def main():
    parser = argparse.ArgumentParser(
        description='Calc LED values. Specify 3 of the 4 values, will calculate the other.')
    parser.add_argument('-vf', help='forward voltage', type=float)
    parser.add_argument('-vb', help="battery voltage", type=float)
    parser.add_argument('-r', help="resistance (ohms)", type=float)
    parser.add_argument('-i', help="current (amps)", type=float)

    args = parser.parse_args()

    vb = args.vb if args.vb else 0
    vf = args.vf if args.vf else 0
    r = args.r if args.r else 0 
    i = args.i if args.i else 0

    if not args.vb:
        # odd case, admittedly.
        vb = i * r + vf
    if not args.vf:
        vf = -(i * r - vb)
    if not args.r:
        r = (vb - vf) / i
    if not args.i:
        i = (vb - vf) / r

    pLED = vf * i
    pRes = (vb - vf) * i
    eff = 1.0 - pRes / (vf * i)
        
    fraction = 16.0
    while pRes > (1.0 / fraction):
        fraction = fraction / 2

    print("vB = {:.2f} volts.".format(vb))
    print("vF = {:.2f} volts.".format(vf))
    print("R  = {:.2f} ohms.".format(r))
    print("I  = {:.3f} amps. ({} mA)".format(i, int(i * 1000.0)))
    print("Power:")
    print("    LED      = {:.2f} watts".format(pLED))
    print("    Resistor = {:.2f} watts".format(pRes))
    print("    Total    = {:.2f} watts".format(pRes + pLED))
    print("    Eff      = {:.1f}%".format(eff * 100))
    if fraction >= 1:
        print("    Standard Resistor: 1/{} watt.".format(int(fraction)))


if __name__ == "__main__":
    main()
