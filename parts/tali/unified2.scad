include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>
T = 4;

N_BAFFLES = nBafflesNeeded(H_BUTTRESS);
M_BAFFLE_FRONT = zLenOfBaffles(N_BAFFLES, H_BUTTRESS) + M_POMMEL_FRONT;

translate([0, 0, M_POMMEL_FRONT]) {
    baffleMCBattery(D_AFT, N_BAFFLES, H_BUTTRESS, D_AFT_RING, 6);
}

translate([0, 0, M_POMMEL_BACK]) {
    speakerHolder(D_POMMEL, M_POMMEL_FRONT - M_POMMEL_BACK, 4, "std28");
}

translate([0, 0, M_BAFFLE_FRONT])
    pcbHolder(D_AFT, T, M_CHAMBER - M_BAFFLE_FRONT, 2, 
    5, [14, 20, 20], [10, 0, 16]);

