use <../shapes.scad>
use <../commonUnified.scad>
include <dim.scad>

N_BATT_BAFFLES = nBafflesNeeded(DZ_BAFFLE);

translate([0, 0, M_START]) {
    speakerHolder(D_INNER, DZ_SPKR, 3.0, "std28");
}

translate([0, 0, M_MC_BATTERY]) {
    baffleMCBattery(D_INNER, N_BATT_BAFFLES, DZ_BAFFLE);
}