include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>

$fn = 80;
T = 4;
FRONT_T = 4;
JUNCTION = 5;
EPS = 0.01;
EPS2 = 2 * EPS;

DRAW_AFT   = false;
DRAW_FRONT = true;
DRAW_CAP   = false;

AFT_ROT = 13;

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
            cylinderKeyJoint(JUNCTION - 0.5);
        }
    }
}


module rods(h, expand=0) {
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
    RING_T = 4;

    /*
        Overall: 35.1
        Lip 3.8
        Total bottom 17.8
        Delta Y: 4.1 (fr) or 5.3 (solder)

        Top:
        Hole (hole): d = 2.2  pos = 21.59, 17.78
            Power: Hole (hole): d = 8.0  pos = 20.32, 10.16
        Hole (mark): d = 0.0  pos = 29.21, 10.16
        Hole (hole): d = 2.2  pos = 21.59, 2.54
        Number of drill =32
        rows/cols = 27,17
        size (after cut) = 32.02, 19.32
    */
    C = 19.32/2;    // center for coordinate conversion
    DYPCB = 7.5;
    MOUNT_DZ = 2.5;  // offset for the PCB mounting holes
    DZ_PCB = RING_T; // + 2.5;

    difference() {
        translate([0, 0, M_BAFFLE_FRONT]) {
            LENZ = M_CHAMBER - M_BAFFLE_FRONT - FRONT_T;
            pcbHolder(  D_AFT, T, 
                        LENZ + EPS2,    // dz 
                        RING_T,         // dzToPCB
                        DYPCB,          // dyPCB
                    [22, 100, LENZ - RING_T], 
                    [
                        // remember axis are flipped from nanopcb
                        // and x is re-oriented. *sigh*
                        [17.28 - C, 21.59 + MOUNT_DZ, "buttress"],
                        [ 2.04 - C, 21.59 + MOUNT_DZ, "buttress"]
                    ]);
            translate([0, 0, 10 + MOUNT_DZ + 2]) {
                columnY(6, DYPCB - 4.1, 6, D_AFT, 4, 2);
            }
            translate([0, 0, 29.21 + MOUNT_DZ + 6]) {
                columnY(6, DYPCB, 6, D_AFT, 4, 2);
            }
            tube(RING_T, do=D_AFT - T, di=D_AFT - 2 * T);
        }
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


if (DRAW_CAP) {
    D = D_AFT;  //31.5;
    H = 13; // 14.5 in theory; space for padding
    D_WASHER = 9.5 + 0.5;
    H_CRYSTAL = 10; // again, space for glue, padding
    D1_CRYSTAL = 11;
    D2_CRYSTAL = 5;

    translate([0, 0, M_CAP]) {
        difference() {
            cylinder(h=H, d=D);

            cylinder(h=H, d=D2_CRYSTAL);
            cylinder(h=H_CRYSTAL, d1=D1_CRYSTAL, d2=D2_CRYSTAL);
 
            rotate([0, 0, AFT_ROT - 90]) {
                // Wire tube. (hold in place)
                hull() {
                    translate([-8, 8, 0]) cylinder(h=H/2, d=6);
                    translate([-4, 4, 0]) cylinder(h=H/2, d=6);
                }
                translate([0, 0, H/2]) hull() {
                    translate([-8, 8, 0]) cylinder(h=H, d=4.6);
                    translate([-4, 4, 0]) cylinder(h=H, d=4.6);
                }
                    
                // Rods
                translate([11, 0, 0]) cylinder(h=H, d=D_WASHER);
                translate([-11, 0, 0]) cylinder(h=H, d=D_WASHER);
            }
       }
    }
}