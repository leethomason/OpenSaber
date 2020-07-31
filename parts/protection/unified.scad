include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>

DRAW_AFT = false;
DRAW_FORE = true;

T_JOINT = 3;
DZ_JOINT = 8;
T = 4;

if (DRAW_AFT) {
    translate([0, 0, M_POMMEL]) {
        speakerHolder(D_INNER, DZ_SPKR, 3.0, "std28");
    }

    translate([0, 0, M_MC_BATTERY]) {
        baffleMCBattery(D_INNER, N_BATT_BAFFLES, DZ_BAFFLE, bridgeStyle=2);
        color("red") battery(D_INNER, "18650");
    }

    translate([0, 0, M_JOINT]) rotate([0, 0, 0])
        keyJoint(DZ_JOINT, D_INNER, D_INNER - T_JOINT, false);

}


if (DRAW_FORE) {
    DZ_POWER_RING = M_PORT - M_JOINT + 10.0;

    difference() {
        translate([0, 0, M_JOINT]) {
            powerPortRing(D_INNER, T, DZ_POWER_RING, M_PORT - M_JOINT);
        }
        translate([0, 0, M_JOINT]) rotate([0, 0, 0])
            keyJoint(DZ_JOINT, D_INNER, D_INNER - T_JOINT, true);
    }
    /*translate([0, 0, M_SWITCH_START]) {
        difference() {
            W = 22;
            tube(h=M_AFT_FRONT - DZ_RING0 - M_SWITCH_START, do=D_INNER, di=D_INNER - T);
            translate([-W/2, 0, 0]) cube(size=[W, 100, 100]);
        }
        switchHolder(D_INNER, M_SWITCH - M_SWITCH_START, 0, 10.5);
    }*/
}