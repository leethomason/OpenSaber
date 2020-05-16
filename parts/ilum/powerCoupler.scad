use <../shapes.scad>
include <dim.scad>

MAIN_H = 8.6;   // height w/o teeth caps
TEETH = 6.1;
TEETH_CAP = 1.0;
EPS = 0.01;
R_MOUNT = 2.54 * 3.0;
BASE_H = MAIN_H - TEETH - TEETH_CAP; // need space for teeth caps to sink in
OVER_H = MAIN_H + TEETH_CAP;

$fn = 120;

module lock(h, cap, angleEps)
{
    N = 3;
    ANGLE = 180.0 / N;
    THETA = ANGLE / 2;
    BETA = 360.0 / N;

    x = cos(THETA - angleEps);
    y = sin(THETA - angleEps);
    M = 20;

    for(r=[0:1]) {
        rotate([0, 0, 90.0 + 180.0*r]) {
            if (!cap) {
                polygonXY(h=h, points=[
                    [0,0], [x*M, -y*M], [x*M, y*M]
                ]);
            }
            else {
                hull() {
                    polygonXY(h=0.1, points=[
                        [0,0], [x*M, -y*M], [x*M, y*M]
                    ]);
                    translate([0, 0, h-0.1]) polygonXY(h=0.1, points=[
                        [0,0], [x*M, -y*M + 1], [x*M, y*M - 1]
                    ]);
                }
            }
        }
    }
}

module coupler(angle)
{
    difference() {
        D_TUBE_INNER = (R_MOUNT - 1.5) * 2;
        D_TUBE_OUTER = (R_MOUNT + 1.5) * 2 + 2.0;

        union() {
            tube(h=BASE_H, di=D_TUBE_INNER, do=D_TUBE_OUTER);
            intersection() {
                tube(h=100, di=D_TUBE_INNER, do=D_TUBE_OUTER);
                union() {
                    translate([0, 0, BASE_H]) 
                        lock(TEETH + TEETH_CAP, false, angle);
                    translate([0, 0, MAIN_H]) 
                        lock(TEETH_CAP, true, angle);
                }
            }
        }
        translate([0, R_MOUNT, 0]) cylinder(h=100, d=1.8);
        translate([0, -R_MOUNT, 0]) cylinder(h=100, d=1.8);

        translate([(D_COUPLER - D_TUBE)/2, 0, -EPS]) cylinder(h=100, d=D_HOLDER);
        translate([-(D_COUPLER - D_TUBE)/2, 0, -EPS]) cylinder(h=100, d=D_HOLDER);
    }
}

translate([0, 0, OVER_H + BASE_H]) mirror([0, 0, -1]) coupler(0);

*color("gray") {
    translate([-UNIT*2, -UNIT/2, 0]) cube(size=[UNIT*4, UNIT, H]);
    translate([-UNIT, -1.5*UNIT, 0]) cube(size=[UNIT*2, UNIT*3, 5]);    // pcb wires
}
// color("gray") cylinder(h=OVER_H, d=2);
echo("Outer D", (R_MOUNT + 1.5) * 2);