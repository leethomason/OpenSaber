use <../commonUnified.scad>
use <../shapes.scad>
use <../inset.scad>
include <dim.scad>

$fn = 80;

module arcCylinder(h, dx, dy)
{
    r = (dx*dx + dy*dy) / (2 * dy);

    intersection() {
        translate([-dx, 0, 0]) cube(size=[dx*2, dy, h]);
        translate([0, dy - r, 0]) cylinder(h=h, r=r);
    }
}

module inner(h)
{
    translate([0, INNER_DY/2, 0])
        arcCylinder(h=h, dx=INNER_DX / 2, dy=(INNER_ARCY - INNER_DY)/2);
    mirror([0, -1, 0])
        translate([0, INNER_DY/2, 0])
            arcCylinder(h=h, dx=INNER_DX / 2, dy=(INNER_ARCY - INNER_DY)/2);
    translate([-INNER_DX/2, -INNER_DY/2, 0])
        cube(size=[INNER_DX, INNER_DY, h]);
}

// z=0 is the pin top
module power(zExtend)
{
    Z_BACK = POWER_DZ - POWER_DZ_PIN;
    translate([-POWER_DX/2, -POWER_DY, -Z_BACK - zExtend])
        cube(size=[POWER_DX, 100, POWER_DZ + zExtend]);
}

// top
module switch()
{
    PIT = 10.0;
    // pits for arms
    translate([SWITCH_DX/2, -PIT, 0]) cube(size=[SWITCH_ARM, 100, SWITCH_DZ]);
    mirror([-1, 0, 0])
        translate([SWITCH_DX/2, -PIT, 0]) cube(size=[SWITCH_ARM, 100, SWITCH_DZ]);
    // switch proper
    translate([-SWITCH_DX/2, -SWITCH_DY, 0]) cube(size=[SWITCH_DX, 100, SWITCH_DZ]);
}

difference() {
    TEST_Z = 20;
    translate([0, 0, -TEST_Z/2]) inner(TEST_Z);
    translate([0, SWITCH_Y, 0]) rotate([-90, 0, 0]) cylinder(h=10, d=D_HOLE);
    translate([0, SWITCH_Y, ACTUAL_POWER_PIN_Z]) power(20);
    translate([0, SWITCH_Y, ACTUAL_POWER_PIN_Z + POWER_DZ_PIN]) switch();
}

