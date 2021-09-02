include <dim.scad>
use <../shapes.scad>

$fn = 60;

Y_DOWN = 4.0;
H_BUTTON = 6.0;
H_PIN = 4.0;
D = 12.0;
R = 0.3;

module torus()
{
    rotate([90, 0, 0]) {
        rotate_extrude(convexity = 10)
            translate([D/2 - R, 0, 0])
                circle(r = R);
    }
}

module body()
{
    difference() {
        translate([0, 0, -20]) tube(h=40, do=40, di=D_INNER);
        rotate([-90, 0, 0]) cylinder(h=20, d=D);
    }
}

difference() {
    union() {
        translate([0, D_INNER/2 - Y_DOWN, 0]) rotate([-90, 0, 0]) cylinder(h=H_BUTTON - R, d=16.0);
        translate([0, D_INNER/2 - Y_DOWN + H_BUTTON - R, 0]) hull() torus();
    }
    body();
    translate([0, D_INNER/2 - Y_DOWN, 0]) rotate([-90, 0, 0]) cylinder(h=H_PIN, d1=4.0, d2=0.0);
}

//torus();
