include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>

DRAW_AFT = true;

$fn = 80;

M_BACK = -9;    // FIXME

N_BAFFLES = nBafflesNeeded(H_BUTTRESS);
EXTRA_BAFFLE = 2.0;
M_BAFFLE_FRONT = zLenOfBaffles(N_BAFFLES, H_BUTTRESS) + M_POMMEL_FRONT + EXTRA_BAFFLE;


if (DRAW_AFT) {
    translate([0, 0, M_POMMEL_FRONT]) {
        baffleMCBattery(D_AFT, N_BAFFLES, H_BUTTRESS, D_AFT_RING, 5.5, EXTRA_BAFFLE);
    }

    translate([0, 0, M_POMMEL_FRONT]) {
        //cylinder(h=M_SPKR_RING - M_POMMEL_FRONT, d=D_AFT_RING);
        //speakerHolder(D_POMMEL, M_BACK, 2, "bass28");
    }
}

