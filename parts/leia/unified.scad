use <../commonUnified.scad>
use <../shapes.scad>
include <dim.scad>

N_BAFFLES = nBafflesNeeded(DZ_BUTTRESS, "18650");


baffleMCBattery(D_INNER_FORE,
    N_BAFFLES,
    DZ_BUTTRESS);

color("red") battery(D_INNER_FORE, "18650");
color("green") translate([0, -10.5, -41.0]) mc();
