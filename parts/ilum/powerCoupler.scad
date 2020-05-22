use <../shapes.scad>
include <dim.scad>
include <dimCoupler.scad>

TOOTH_THETA = 3.0;
EPS = 0.01;
THETA_EPS = 1.0;

$fn = 120;

module tooth(angle, angleWidth)
{
    angleWidth = angleWidth * 0.5;
    x0 = cos(angle - angleWidth);
    y0 = sin(angle - angleWidth);

    x1 = cos(angle + angleWidth);
    y1 = sin(angle + angleWidth);

    x0t = cos(angle - angleWidth + TOOTH_THETA);
    y0t = sin(angle - angleWidth + TOOTH_THETA);

    x1t = cos(angle + angleWidth - TOOTH_THETA);
    y1t = sin(angle + angleWidth - TOOTH_THETA);

    M = 20;

    polygonXY(h=TOOTH_BASE, points=[
        [0,0], [x0*M, y0*M], [x1*M, y1*M]
    ]);
    translate([0, 0, TOOTH_BASE]) hull() {
        polygonXY(h=0.1, points=[
            [0,0], [x0*M, y0*M], [x1*M, y1*M]
        ]);
        translate([0, 0, TOOTH_CAP-0.1]) polygonXY(h=0.1, points=[
            [0,0], [x0t*M, y0t*M], [x1t*M, y1t*M]
        ]);
    }
}

module coupler(teeth)
{
    difference() {

        union() {
            tube(h=TOOTH_BASE_H, di=D_COUPLER_INNER, do=D_COUPLER_OUTER);
            intersection() {
                tube(h=100, di=D_COUPLER_INNER, do=D_COUPLER_OUTER);
                union() {
                    for(t = teeth) {
                        tooth(t[0], t[1]);
                    }
                }
            }
        }
        translate([(D_COUPLER - D_TUBE)/2, 0, -EPS]) cylinder(h=100, d=D_HOLDER);
        translate([-(D_COUPLER - D_TUBE)/2, 0, -EPS]) cylinder(h=100, d=D_HOLDER);
    }
}

// 0
difference() {
    translate([0, 0, TOOTH_HEIGHT + TOOTH_BASE_H]) mirror([0, 0, -1]) coupler([[-90, 60], [90, 60]]);
    rotate([0, 0, 90]) translate([R_COUPLER_MOUNT, 0, 0]) cylinder(h=100, d=D_M2);
    rotate([0, 0, -90]) translate([R_COUPLER_MOUNT, 0, 0]) cylinder(h=100, d=D_M2);
}

// 1
*difference() {
    color("plum") coupler([[0, 120 - THETA_EPS], [180, 120 - THETA_EPS]]);
    rotate([0, 0, 45]) translate([R_COUPLER_MOUNT, 0, 0]) cylinder(h=100, d=D_M2);
    rotate([0, 0, 180 + 45]) translate([R_COUPLER_MOUNT, 0, 0]) cylinder(h=100, d=D_M2);
    rotate([0, 0, -45]) translate([R_COUPLER_MOUNT, 0, 0]) cylinder(h=100, d=D_M2);
    rotate([0, 0, 180 - 45]) translate([R_COUPLER_MOUNT, 0, 0]) cylinder(h=100, d=D_M2);
}

*color("gray") {
    translate([-UNIT*2, -UNIT/2, 0]) cube(size=[UNIT*4, UNIT, H]);
    translate([-UNIT, -1.5*UNIT, 0]) cube(size=[UNIT*2, UNIT*3, 5]);    // pcb wires
}
// color("gray") cylinder(h=TOOTH_HEIGHT, d=2);
echo("Outer D", (R_COUPLER_MOUNT + 1.5) * 2);