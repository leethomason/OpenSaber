include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>

$fn = 60;
T = 4;
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

if (DRAW_FRONT) {
    /*
        Assuming 3 things on PCB:
        1. power port
        2. switch
        3. segment display
    */
    difference() {
        translate([0, 0, M_BAFFLE_FRONT])
            pcbHolder(D_AFT, T, M_CHAMBER - M_BAFFLE_FRONT, 2, 5, 
                    [14, 20, 20], [10, 0, 16]);
        translate([0, 0 , M_BAFFLE_FRONT - EPS])
            cylinderKeyJoint(JUNCTION + EPS2);
    }

    /*
        And now a loose couple to the crystal chamber.
    */
    translate([0, 0, M_CHAMBER - T]) {
        W = 10;
        difference() {
            translate([0, 0, EPS]) cylinder(h=T, d=D_AFT);
            /*
            intersection() {
                cylinder(h=T, d=D_AFT);
                translate([-W/2, -50, 0])
                    cube(size=[W, 100, T]);
            }
            */
            rotate([90, 0, 180])
                dotstarLED(2, 10);

            // Wire tube
            translate([6, -6, 0])
                cylinder(h=10, d=6);
            // Rods
            translate([10, 3, 0])
                cylinder(h=10, d=3.6);
            translate([-10, -3, 0])
                cylinder(h=10, d=3.6);
        }
    }
}

