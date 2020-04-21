use <../commonUnified.scad>
include <dim.scad>

T = 4.0;

SWITCH_DX = 6.5;
SWITCH_LEGS_DX = 8.0;
SWITCH_DX0 = 3.0;
SWITCH_D = 6.0;
SWITCH_DY = 7.5;
SWITCH_DZ = 9.5;
SWITCH_TRIM = 1.0;

PILLAR_DZ = SWITCH_DY;
PILLAR_DX = 3.0;

RING_DZ = 13.0;
RING_CENTER = RING_DZ / 2;

$fn = 40;

difference() {
    union() {
        powerPortRing(D_INNER_AFT, T, RING_DZ, RING_CENTER);
        intersection() {
            cylinder(h=RING_DZ, d=D_INNER_AFT);
            union() {
                translate([D_INNER_AFT/2 - SWITCH_DX - PILLAR_DX, -100, RING_CENTER - PILLAR_DZ/2])
                    cube(size=[200, 101, PILLAR_DZ]);
                translate([D_INNER_AFT/2 - SWITCH_DX + SWITCH_DX0, -100, 0])
                    cube(size=[100, 200, 100]);
            }
        }
    }

    // Cut out body.
    EXTRA = 1.0;
    translate([D_INNER_AFT/2 - SWITCH_DX - EXTRA, -SWITCH_DY/2, RING_CENTER - SWITCH_DZ/2])
        cube(size=[SWITCH_DX0 + EXTRA, 100, SWITCH_DZ]);

    // Legs
    //translate([D_INNER_AFT/2 - SWITCH_LEGS_DX, -SWITCH_DY/2, RING_CENTER - SWITCH_DZ/2-1])
    //    cube(size=[4, 100, 1]);
    //translate([D_INNER_AFT/2 - SWITCH_LEGS_DX, -SWITCH_DY/2, RING_CENTER + SWITCH_DZ/2])
    //    cube(size=[4, 100, 1]);

    *hull() {
        translate([D_INNER_AFT/2 - SWITCH_DX + SWITCH_DX0, 0, RING_CENTER])
            rotate([0, 90, 0]) cylinder(h=100, d=SWITCH_D);
        translate([D_INNER_AFT/2 - SWITCH_DX + SWITCH_DX0, 0, RING_CENTER])
            rotate([0, 90, 0]) cylinder(h=100, d=SWITCH_D);
    }

    translate([D_INNER_AFT/2 - SWITCH_DX + SWITCH_DX0, -SWITCH_DY/2, RING_CENTER - SWITCH_D/2])
        cube(size=[100, 100, SWITCH_D]);

    translate([D_INNER_AFT/2 - SWITCH_TRIM, -100, 0])
        cube(size=[200, 200, 200]);
    
    //*translate([D_INNER_AFT/2 - SWITCH_DX, -SWITCH_DY/2, RING_CENTER - SWITCH_DZ/2])
    //    cube(size=[100, SWITCH_DY, SWITCH_DZ]);
}

*intersection() {
    cylinder(h=RING_DZ, d=D_INNER_AFT);
    difference() {
        translate([D_INNER_AFT/2 - SWITCH_DX - PILLAR_DX, -100, RING_CENTER - PILLAR_DZ/2])
            cube(size=[200, 200, PILLAR_DZ]);
        translate([D_INNER_AFT/2 - SWITCH_DX, -SWITCH_DY/2, RING_CENTER - PILLAR_DZ/2])
            cube(size=[100, SWITCH_DY, SWITCH_DZ]);
    }
}

