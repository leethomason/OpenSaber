include <dim.scad>
use <../shapes.scad>

$fn = 80;
T = 4;
DZ = 50;
PITCH = 7.0;
DRILL = 1.4;
CUT_Y = 18;
PAD_Y = 5;

OUTER = D_OUTER + 0.4;

difference() {
    union() {
        cylinder(h=DZ, d=OUTER + T);
        translate([0, -PAD_Y, 0])
            cube(size=[24, PAD_Y*2, DZ]);
    }
    cylinder(h=200, d=OUTER);
    translate([0, 0, DZ/2])
        zCapsule(DZ_PORT, DZ_SWITCH, D_CAPSULE/2);
    for(i=[0:3]) {
        translate([0, 0, DZ/2 - PITCH*1.5 + i * PITCH]) {
            rotate([0, 90, 0])
                cylinder(h=50, d=DRILL);
        } 
    }
    mirror([-1, 0, 0])
        translate([0, -CUT_Y, 0])
            cube(size=[50, CUT_Y*2, 200]);
}

