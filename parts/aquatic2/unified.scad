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

EXTRA_BAFFLE = 2;
N_BAFFLES = nBafflesNeeded(H_BUTTRESS);
M_BAFFLE_FRONT = zLenOfBaffles(N_BAFFLES, H_BUTTRESS) + M_POMMEL_FRONT + EXTRA_BAFFLE;


if (DRAW_AFT) {
    translate([0, 0, M_POMMEL_FRONT]) {
        baffleMCBattery(D_AFT, N_BAFFLES, H_BUTTRESS, D_AFT_RING, 5.5);
    }
    translate([0, 0, M_BAFFLE_FRONT - EXTRA_BAFFLE]) {
        oneBaffle(D_AFT, EXTRA_BAFFLE, bridge=false);
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


if (DRAW_FRONT) {
    /*
        Assuming 3 things on PCB:
        1. power port
        2. switch
        3. segment display
    */
    RING_T = 4 - EXTRA_BAFFLE;

    /*
        FIXME
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
    MOUNT_DZ = 4.5;  // offset for the PCB mounting holes
    DZ_PCB = RING_T; // + 2.5;

    difference() {
        translate([0, 0, M_BAFFLE_FRONT]) {
            LENZ = M_CHAMBER - M_BAFFLE_FRONT - FRONT_T;
            pcbHolder(  D_AFT, T, 
                        LENZ + EPS2,    // dz 
                        RING_T,         // dzToPCB
                        DYPCB,          // dyPCB
                    [22, 100, 50],      // FIXME 
                    [
                        // remember axis are flipped from nanopcb
                        // and x is re-oriented. *sigh*
                        [17.28 - C, 21.59 + MOUNT_DZ, "buttress"],
                        [ 2.04 - C, 21.59 + MOUNT_DZ, "buttress"]
                    ]);

            intersection() {
                cylinder(h=H_FAR, d=D_AFT + EPS2);
                union() {
                    translate([0, -D_AFT/2, MOUNT_DZ + RING_T + 6]) {
                        difference() {
                            color("olive") 
                                bridge(D_AFT, D_AFT/2 + DYPCB - 4.1, 4, 10);
                        }
                    }
                    translate([0, -D_AFT/2, MOUNT_DZ + RING_T + 26]) {
                        bridge(D_AFT, D_AFT/2 + DYPCB, 4, 10);
                    }
                }
            }
            tube(RING_T, do=D_AFT - T, di=D_AFT - 2 * T);
        }
        translate([0, 0 , M_BAFFLE_FRONT - EPS])
        intersection() {
            tube(JUNCTION, do=D_AFT+EPS, di=D_AFT - T - EPS);
            cylinderKeyJoint(JUNCTION);
        }

        // Flat bottom for printing
        translate([-50, -D_AFT/2, M_BAFFLE_FRONT]) 
            cube(size=[100, 0.5, 100]);

        // Aesthetics
        /*
        translate([0, 0, M_BAFFLE_FRONT + 8 ]) 
            capsule(-110, -80, 2, true);
        translate([0, 0, M_BAFFLE_FRONT + 20 ]) 
            capsule(-110, -80, 2, true);
        translate([0, 0, M_BAFFLE_FRONT + 40 ]) 
            capsule(-110, -80, 2, true);
        for(i=[0:4]) {
            translate([0, 0, M_BAFFLE_FRONT + 8 + i*6])
                capsule(-170, 170, 2);
        }
        */
        // FIXME
        rotate([0, 0, 180]) translate([0, D_AFT/2 - 5, 50]) {
            dotstarLED(8, 10, 10.0);
            dotstarStrip(8, pitch=10.0);    // fixme
        }

    }
}


*color("red") translate([0, 0, M_POMMEL_FRONT])
    battery(D_AFT);
