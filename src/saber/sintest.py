import math

angle = 0.0

while angle <= 1.0:
    vecx = math.cos(angle)
    vecy = math.sin(angle)

    cx = 1.0 - vecx
    cy = 0.0 - vecy
    c = math.sqrt(cx * cx + cy * cy)

    print("angle={:.3f} vec={:.3f}, {:.3f} c={:.3f}".format(angle, vecx, vecy, c))

    angle += 0.1