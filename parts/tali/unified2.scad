include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>

$fn = 80;
T = 4;
FRONT_T = 4;
JUNCTION = 5;
EPS = 0.01;
EPS2 = 2 * EPS;

DRAW_AFT = true;
DRAW_FRONT = false;

N_BAFFLES = nBafflesNeeded(H_BUTTRESS);
M_BAFFLE_FRONT = zLenOfBaffles(N_BAFFLES, H_BUTTRESS) + M_POMMEL_FRONT;

if (DRAW_AFT) {
    translate([0, 0, M_POMMEL_FRONT]) {
        baffleMCBattery(D_AFT, N_BAFFLES, H_BUTTRESS, D_AFT_RING, 5.5);
    }

    translate([0, 0, M_POMMEL_BACK]) {
        speakerHolder(D_POMMEL, M_POMMEL_FRONT - M_POMMEL_BACK, 2, "bass28");
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
        if (expand == 0) {
            translate([-8, 8, 0])
                cylinder(h=h, d=4.6);
            translate([-8, 8, 2])
                cylinder(h=h, d=6);
        }

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
    C = 19.32/2;    // center for coordinate conversion
    RING_T = 4;
    MOUNT_DZ = -RING_T;  // offset for the PCB mounting holes

    difference() {
        translate([0, 0, M_BAFFLE_FRONT])
            pcbHolder(  D_AFT, T, 
                        M_CHAMBER - M_BAFFLE_FRONT - FRONT_T + EPS2,    // dz 
                        RING_T, // dzToPCB
                        6.5, // dyPCB
                    // Actual size: 19.4, 37.2
                    [20, 20, M_CHAMBER - M_BAFFLE_FRONT - FRONT_T - RING_T], 
                    [
                        // remember axis are flipped from nanopcb
                        // and x is re-oriented. *sigh*
                        [17.78 - C, 25.40 + MOUNT_DZ, "buttress"],
                        [16.51 - C,  2.54 + MOUNT_DZ, "buttress"],
                        [10.16 - C, 34.29 + MOUNT_DZ, "pillar"],
                        [ 3.81 - C,  2.54 + MOUNT_DZ, "buttress"],
                        [ 2.54 - C, 25.40 + MOUNT_DZ, "buttress"]
                    ]);
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

