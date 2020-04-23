use <../commonUnified.scad>
use <../shapes.scad>
use <../inset.scad>
include <dim.scad>

$fn = 40;
DRAW_AFT = false;
DRAW_FORE = true;

EPS = 0.01;
ESP2 = 2 * EPS;

PLATE_TRIM = 1.0;
JOINT = 8;
T = 4;

module dotStarCut()
{
    mirror([-1, 0, 0]) {
        dotstarZ = dotstarStripZLen(4);
        dotstarX = 12.4;    // of the strip, not the LED
        rotate([0, 0, 10])  // FIXME correct rotation
        {
            // at least 3.0!
            translate([D_INNER/2 - 3.5, 0, 0]) 
                polygonXY(h=dotstarZ, points = [
                    [0, -dotstarX/2],
                    [10, -dotstarX/2 + 10],
                    [10, dotstarX/2 + 10],
                    [0, dotstarX/2]
                ]);
            
            // Wire access
            DX_WIRE = 9;
            *translate([D_INNER_AFT/2 - 5.0, -DX_WIRE/2, Z_START_SECTION + DZ_BUTTRESS * 3])
                cube(size=[20, DX_WIRE, DZ_BUTTRESS]);
        }
    }
}


if (DRAW_AFT) {
    translate([0, 0, M_START]) {
        speakerHolder(D_INNER, DZ_SPKR, 3.0, "std28");
    }

    difference() {
        translate([0, 0, M_MC_BATTERY]) {
            baffleMCBattery(D_INNER, N_BATT_BAFFLES, DZ_BAFFLE, bridgeStyle=2);
            *color("red") battery(D_INNER, "18650");
        }
    }
    translate([0, 0, M_JOINT]) rotate([0, 0, 0])
        keyJoint(JOINT, D_INNER, D_INNER - 4, false);

}

if (DRAW_FORE) {
    TOP_FLATTEN = 2.0;

    difference() {
        union() {
            translate([0, 0, M_JOINT]) {
                powerPortRing(D_INNER, T, DZ_POWER_RING, DZ_POWER_RING/2, addY=TOP_FLATTEN);
            }

            translate([0, 0, M_BOLT_START]) {
                boltRing(D_INNER, T, DZ_BOLT, DZ_BOLT/2);
            }

            translate([0, 0, M_SWITCH_START]) {
                difference() {
                    W = 22;
                    tube(h=M_AFT_THREAD_FRONT - M_SWITCH_START, do=D_INNER, di=D_INNER - T, $fn=80);
                    translate([-W/2, 0, 0]) cube(size=[W, 100, 100]);
                }
                switchHolder(D_INNER, M_SWITCH - M_SWITCH_START, 0, 10.0);
            }

            // toughen up the sides
            difference() {
                THICK = 5.0;
                translate([0, 0, M_JOINT]) intersection() {
                    cylinder(d=D_INNER, h=DZ_FORE);
                    union() {
                        translate([D_INNER/2 - THICK, -50, 0]) cube(size=[10, 100, 100]);
                        mirror([-1, 0, 0]) translate([D_INNER/2 - THICK, -50, 0]) cube(size=[10, 100, 100]);
                    }
                }
                cylinder(h=100, d=D_INNER - 8);
            }
        }
        // Flat area for switch plate.
        translate([-50, D_INNER/2 - TOP_FLATTEN, M_JOINT]) // + PLATE_TRIM])
            cube(size=[100, 10, 100]);
        
        translate([0, 0, M_JOINT]) rotate([0, 0, 0])
            keyJoint(JOINT, D_INNER, D_INNER - 4, true);

        //translate([0, 0, M_JOINT + 8]) dotStarCut();

        // Flat bottom
        translate([-50, -D_INNER/2 - EPS, 0]) cube(size=[100, 1, 500]);
    }
}

INSET_W = 14.0;
*difference() {
    translate([-INSET_W/2, D_INNER/2, M_JOINT + PLATE_TRIM])
        roundedRect(size=[INSET_W, 1, M_AFT_THREAD_FRONT - M_JOINT - 2.0*PLATE_TRIM], r=1.6, up="y");
        
    translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=100, d=8.0);
    translate([0, 0, M_BOLT]) rotate([-90, 0, 0]) cylinder(h=100, d=4.2);
    translate([0, 0, M_PORT]) rotate([-90, 0, 0]) cylinder(h=100, d=11.0);
}

*color("blue") translate([0, 0, M_START-1]) cylinder(h=1, d=D_OUTER);
*color("blue") translate([0, 0, M_AFT_THREAD_FRONT]) cylinder(h=1, d=D_OUTER);
*color("blue") translate([0, 0, M_AFT_FRONT]) cylinder(h=1, d=D_OUTER);




