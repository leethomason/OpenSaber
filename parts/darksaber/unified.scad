use <../commonUnified.scad>
use <../shapes.scad>
use <../inset.scad>
include <dim.scad>

DRAW_AFT = false;
DRAW_FORE = true;

ARC_INNER_DX = INNER_DX / 2;
ARC_INNER_DY = (INNER_ARCY - INNER_DY) / 2;

INNER_R = (ARC_INNER_DX * ARC_INNER_DX + ARC_INNER_DY * ARC_INNER_DY) / (2 * ARC_INNER_DY);
//echo("INNER_R", INNER_R);
SWITCH_Y_OFFSET = INNER_R - yAtX(POWER_DX/2, INNER_R);
SWITCH_Y = INNER_ARCY/2 - SWITCH_Y_OFFSET;
//echo("SWITCH_Y_OFFSET", SWITCH_Y_OFFSET);

INNER_SCALE = 0.9;


$fn = 80;

module arcCylinder(h, dx, dy)
{
    r = (dx*dx + dy*dy) / (2 * dy);

    intersection() {
        translate([-dx, 0, 0]) cube(size=[dx*2, dy, h]);
        translate([0, dy - r, 0]) cylinder(h=h, r=r);
    }
}

module inner(extend=0)
{
    translate([0, INNER_DY/2, M_CARRIAGE - extend/2])
        arcCylinder(h=DZ_CARRIAGE + extend, dx=INNER_DX / 2, dy=(INNER_ARCY - INNER_DY)/2);
    mirror([0, -1, 0])
        translate([0, INNER_DY/2, M_CARRIAGE - extend/2])
            arcCylinder(h=DZ_CARRIAGE + extend, dx=INNER_DX / 2, dy=(INNER_ARCY - INNER_DY)/2);
    translate([-INNER_DX/2, -INNER_DY/2, M_CARRIAGE - extend/2])
        cube(size=[INNER_DX, INNER_DY, DZ_CARRIAGE + extend]);
}

// z=0 is the pin top
module power(zExtend)
{
    Z_BACK = POWER_DZ - POWER_DZ_PIN;
    translate([-POWER_DX/2, -POWER_DY, -Z_BACK - zExtend])
        cube(size=[POWER_DX, 100, POWER_DZ + zExtend]);
}

module switchArm()
{
    PIT = 10.0;
    translate([SWITCH_DX/2, -PIT, (SWITCH_DZ - SWITCH_ARM_DZ)/2]) 
        cube(size=[SWITCH_ARM, 100, SWITCH_ARM_DZ]);
}

// top
module switch()
{
    switchArm();
    mirror([-1, 0, 0]) switchArm();

    translate([-SWITCH_DX/2, -SWITCH_DY, 0]) cube(size=[SWITCH_DX, 100, SWITCH_DZ]);
}

module carriage()
{
    difference() {
        inner();
        scale([INNER_SCALE, INNER_SCALE, 1]) inner(1);

        for(i=[1:7]) {
            translate([0, 0, M_CARRIAGE + 2 + i*20]) hull() {
                S = 10;
                translate([-50, 2, 0]) rotate([0, 90, 0]) cylinder(h=100, d=S);
                translate([-50, 50, -S/2]) cube(size=[100, 1, S]);
            }
        }
    }
}

if (DRAW_FORE) {
    difference() {
        intersection() {
            inner();
            translate([-50, -50, M_SWITCH_SECTION_START]) cube(size=[100, 100, DZ_SWITCH_SECTION]);
        }
        translate([0, SWITCH_Y, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=10, d=D_HOLE);
        translate([0, SWITCH_Y, ACTUAL_POWER_PIN_Z]) power(2);
        translate([0, SWITCH_Y, ACTUAL_POWER_PIN_Z + POWER_DZ_PIN]) switch();

        intersection() {
            scale([INNER_SCALE, INNER_SCALE, 1]) inner(1);
            DZ  = M_SWITCH - M_SWITCH_SECTION_START - 8.0;
            union() {
                translate([-50, -50, M_SWITCH_SECTION_START]) cube(size=[100, 100, DZ]);
                translate([-50, -50, M_SWITCH_SECTION_START]) cube(size=[100, 50 - 2.0, 100]);
            }
        }

        // hint of flattening
        translate([-50, -INNER_ARCY/2, 0]) cube(size=[200, 0.5, 200]);
    }
}

if (DRAW_AFT) {
    difference() {
        carriage();

        translate([-50, -50, M_SWITCH_SECTION_START]) cube(size=[100, 100, 1000]);

        // hint of flattening
        translate([-50, -INNER_ARCY/2, 0]) cube(size=[200, 0.5, 200]);
    }
}

color("red") {
    translate([0, INNER_ARCY/2, M_BOLT_1]) rotate([-90, 0, 0]) cylinder(h=4, d=D_BOLT);
    *translate([0, INNER_ARCY/2, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=4, d=D_SWITCH);
    translate([0, INNER_ARCY/2, M_BOLT_2]) rotate([-90, 0, 0]) cylinder(h=4, d=D_BOLT);
    translate([0, INNER_ARCY/2, M_BOLT_3]) rotate([-90, 0, 0]) cylinder(h=4, d=D_BOLT);
}