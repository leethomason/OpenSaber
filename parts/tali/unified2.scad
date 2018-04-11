include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>

$fn = 60;
T = 4;
FRONT_T = 2;
JUNCTION = 7;
EPS = 0.01;
EPS2 = 2 * EPS;

DRAW_AFT = false;
DRAW_FRONT = true;

N_BAFFLES = nBafflesNeeded(H_BUTTRESS);
M_BAFFLE_FRONT = zLenOfBaffles(N_BAFFLES, H_BUTTRESS) + M_POMMEL_FRONT;

if (DRAW_AFT) {
    translate([0, 0, M_POMMEL_FRONT]) {
        baffleMCBattery(D_AFT, N_BAFFLES, H_BUTTRESS, D_AFT_RING, 6);
    }

    translate([0, 0, M_POMMEL_BACK]) {
        speakerHolder(D_POMMEL, M_POMMEL_FRONT - M_POMMEL_BACK, 4, "std28");
    }

    translate([0, 0, M_BAFFLE_FRONT]) {
        intersection() {
            tube(JUNCTION, do=D_AFT, di=D_AFT - T);
            cylinderKeyJoint(JUNCTION);
        }
    }
}


module rods(h, expand=0) {
    AFT_ROT = 12;
    rotate([0, 0, AFT_ROT - 90]) {
        if (expand == 0)
            rotate([90, 0, 0])
                dotstarLED(2, h);

        // Wire tube
        if (expand == 0)
            translate([-8, 8, 0])
                cylinder(h=h, d=6);

        // Rods
        translate([11, 0, 0])
            cylinder(h=h, d=expand > 0 ? expand : 3.6);
        translate([-11, 0, 0])
            cylinder(h=h, d=expand > 0 ? expand : 3.6);
    }
}


if (DRAW_FRONT) {
    /*
        Assuming 3 things on PCB:
        1. power port
        2. switch
        3. segment display
    */
    difference() {
        translate([0, 0, M_BAFFLE_FRONT])
            pcbHolder(D_AFT, T, M_CHAMBER - M_BAFFLE_FRONT - FRONT_T, 2, 5, 
                    [14, 20, 20], [10, 0, 16]);
        translate([0, 0 , M_BAFFLE_FRONT - EPS])
        intersection() {
            tube(JUNCTION, do=D_AFT+EPS, di=D_AFT - T - EPS);
            cylinderKeyJoint(JUNCTION);
        }
        NUT_T = 5;
        translate([0, 0, M_CHAMBER - FRONT_T - NUT_T])
            rods(50, 12);

        // flat bottom
        translate([-50, -D_AFT/2, M_BAFFLE_FRONT]) 
            cube(size=[100, 0.5, 100]);
    }

    /*
        And now a loose couple to the crystal chamber.
    */
    AFT_ROT = 12;
    translate([0, 0, M_CHAMBER - FRONT_T]) {
        W = 10;
        difference() {
            translate([0, 0, EPS]) cylinder(h=FRONT_T, d=D_AFT);
            rods(10);

            // flat bottom
            translate([-50, -D_AFT/2, 0]) 
                cube(size=[100, 0.5, 100]);
        }
    }
}

