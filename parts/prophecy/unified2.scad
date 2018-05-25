use <../shapes.scad>
include <dim.scad>
use <../commonUnified.scad>

$fn = 90;

DRAW_AFT = true;
DRAW_FRONT = true;

T = 4;
JUNCTION = 6;
EXTRA_BAFFLE = 2;
N_BAFFLES = nBafflesNeeded(H_BUTTRESS);
M_BAFFLE_FRONT = zLenOfBaffles(N_BAFFLES, H_BUTTRESS) + M_POMMEL_FRONT + EXTRA_BAFFLE;

if (DRAW_AFT) {
    translate([0, 0, M_POMMEL_FRONT]) {
        baffleMCBattery(D_AFT, N_BAFFLES, H_BUTTRESS, D_AFT_RING, 5.5, EXTRA_BAFFLE);
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
    translate([0, 0, M_BAFFLE_FRONT]) {
        difference() {
            W = 14;
            DZ = M_TRANSITION - M_BAFFLE_FRONT;
            tube(h=DZ, do=D_AFT, di=D_AFT - T);
            translate([-W/2, -50, 0]) {
                cube(size=[W, 100, DZ]);
            }
        }
    }
}

if (DRAW_FRONT) {
}
