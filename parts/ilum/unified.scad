use <../commonUnified.scad>
use <../shapes.scad>
use <../inset.scad>
include <dim.scad>

translate([0, 0, M_START]) {
    speakerHolder(D_INNER, DZ_SPKR, 3.0, "std28");
}

translate([0, 0, M_MC_BATTERY]) {
    baffleMCBattery(D_INNER, N_BATT_BAFFLES, DZ_BAFFLE, bridgeStyle=2);
    *color("red") battery(D_INNER, "18650");
}

translate([0, 0, M_JOINT]) {
    powerPortRing(D_INNER, 4, DZ_POWER_RING, DZ_POWER_RING/2);
}

translate([0, 0, M_BOLT_START]) {
    boltRing(D_INNER, 4, DZ_BOLT, DZ_BOLT/2);
}

translate([0, 0, M_SWITCH_START]) {
    difference() {
        W = 22;
        tube(h=M_AFT_THREAD_FRONT - M_SWITCH_START, do=D_INNER, di=D_INNER - 4, $fn=80);
        translate([-W/2, 0, 0]) cube(size=[W, 100, 100]);
    }
    switchHolder(D_INNER, M_SWITCH - M_SWITCH_START, 0, 5.5);
    //tube(h=M_AFT_THREAD_FRONT - M_SWITCH_START, do=D_INNER, di=D_INNER - 4);
}

*color("blue") translate([0, 0, M_START-1]) cylinder(h=1, d=D_INNER);
*color("blue") translate([0, 0, M_AFT_FRONT]) cylinder(h=1, d=D_INNER);
*color("blue") translate([0, 0, M_AFT_FRONT - 10.0]) cylinder(h=1, d=D_INNER);




