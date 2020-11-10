include <apprenticeV4.scad>
use <../commonUnified.scad>
use <../shapes.scad>

module inner()
{
    cylinder(h=DZ_AFT, d=D_AFT_INNER);
    translate([0, 0, DZ_AFT]) {
        cylinder(h=DZ_TOTAL - DZ_AFT, d=D_FORE_INNER);
    }
}

N_BAFFLES = 10.0;
DZ_BAFFLE = 3.8;
T = 4.0;

PLATE_PAD = 2.5;
M_BOLT = M_SWITCH_CENTER;
M_PORT = M_SWITCH_CENTER - DZ_PLATE / 2 + PLATE_PAD + D_PLATE_PORT / 2;
M_SWITCH = M_SWITCH_CENTER + DZ_PLATE / 2 - PLATE_PAD - D_PLATE_SWITCH / 2;

M_JOINT = zLenOfBaffles(N_BAFFLES, DZ_BAFFLE) + M_MC + DZ_SPEAKER;
M_BOLT_START = M_PORT + 6.0;
M_SWITCH_START = (M_BOLT + M_SWITCH) / 2;

DZ_PORT_SECTION = M_BOLT_START - M_JOINT;
DZ_BOLT_SECTION = M_SWITCH_START - M_BOLT_START;
DZ_SWITCH_SECTION = M_HEAD_SINK_THREAD - M_SWITCH_START;

intersection() {
    inner();
    union() {
        translate([0, 0, M_MC]) {
            speakerHolder(D_AFT_INNER, DZ_SPEAKER, 2.0, "std28");
        }
        translate([0, 0, M_MC + DZ_SPEAKER]) {
            baffleMCBattery(D_AFT_INNER, 
                            N_BAFFLES,
                            DZ_BAFFLE);
        }
    }
}

union() {
    translate([0, 0, M_JOINT])
        tube(h=M_FORE - M_JOINT, do=D_AFT_INNER, di=D_FORE_INNER - T);

    translate([0, 0, M_JOINT]) {
        powerPortRing(D_FORE_INNER, T, DZ_PORT_SECTION, 
                      M_PORT - M_JOINT);
    }
    translate([0, 0, M_BOLT_START]) {
        boltRing(D_FORE_INNER, T, DZ_BOLT_SECTION, M_BOLT - M_BOLT_START);
    }
    translate([0, 0, M_SWITCH_START]) {
        difference() {
            tube(h=DZ_SWITCH_SECTION, do=D_FORE_INNER, di=D_FORE_INNER - T);
            translate([0, 0, M_SWITCH - M_SWITCH_START]) rotate([-90, 0, 0]) cylinder(h=20, d=8.0);
        }
    }
}

translate([0, 0, M_MC + DZ_SPEAKER]) {
    color("red") battery(D_AFT_INNER);
}
