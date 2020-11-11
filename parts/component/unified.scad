include <apprenticeV4.scad>
use <../commonUnified.scad>
use <../shapes.scad>

DRAW_AFT = false;
DRAW_FORE = true;
DRAW_PLATE = false;

EPS = 0.01;

$fn = 60;

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

M_JOINT = zLenOfBaffles(N_BAFFLES, DZ_BAFFLE) + M_MC + DZ_SPEAKER;
M_BOLT_START = M_PORT + 6.5;
M_SWITCH_START = M_BOLT_START + 8.0;

DZ_PORT_SECTION = M_BOLT_START - M_JOINT;
DZ_BOLT_SECTION = M_SWITCH_START - M_BOLT_START;
DZ_SWITCH_SECTION = M_HEAD_SINK_THREAD - M_SWITCH_START;

if (DRAW_AFT) {
    union() {
        translate([0, 0, M_MC]) {
            speakerHolder(D_AFT_INNER, DZ_SPEAKER, 2.0, "std28");
        }
        translate([0, 0, M_MC + DZ_SPEAKER]) {
            baffleMCBattery(D_AFT_INNER, 
                            N_BAFFLES,
                            DZ_BAFFLE,
                            bridgeStyle=2);
        }
    }
    color("olive") translate([0, 0, M_JOINT]) 
        keyJoint(KEYJOINT, D_AFT_INNER, D_AFT_INNER - KEYJOINT_T, false);
}

if (DRAW_FORE) {
    difference() {
        union() {
            translate([0, 0, M_JOINT]) {
                tube(h=M_FORE - M_JOINT, do=D_AFT_INNER, di=D_FORE_INNER - T);
            }
            translate([0, 0, M_JOINT]) {
                powerPortRing(D_FORE_INNER, T, DZ_PORT_SECTION, 
                            M_PORT - M_JOINT,
                            counter=false);
            }
            translate([0, 0, M_BOLT_START]) {
                boltRing(D_FORE_INNER, T, DZ_BOLT_SECTION, M_BOLT - M_BOLT_START);
            }            
            translate([0, 0, M_SWITCH_START]) {
                tactileRing(D_FORE_INNER, T, DZ_SWITCH_SECTION, M_SWITCH - M_SWITCH_START);
            }
            translate([0, 0, M_HEAD_SINK_THREAD]) tube(h=DZ_HEAT_SINK_THREAD, do=D_FORE_INNER, di=dynamicHeatSinkThread());
        }
        // Flat top
        translate([-50, D_FORE_INNER/2 - DY_FLAT, M_FORE]) {
            cube(size=[100, 10, M_HEAD_SINK_THREAD - M_FORE]);
        }

        // flat bottom
        translate([-50, -D_FORE_INNER/2 - 10, M_JOINT]) {
            cube(size=[100, 10.5, 200]);
        }

        // Bottom access
        hull() {
            translate([0, -D_FORE_INNER/2 + T, M_JOINT + 16.0]) rotate([90, 0, 0]) cylinder(h=20, d=10.0);
            translate([0, -D_FORE_INNER/2 + T, M_SWITCH]) rotate([90, 0, 0]) cylinder(h=20, d=10.0);
        }
        translate([0, 0, M_JOINT]) keyJoint(KEYJOINT, D_AFT_INNER, D_AFT_INNER - KEYJOINT_T, true);

        // Side vents
        intersection() {
            translate([0, 0, M_JOINT]) tube(h=100, do=100, di=D_FORE_INNER - T - 0.1);
            for(i=[0:2])
                translate([0, 0, M_JOINT + 16 + i*8]) capsule(70, 110, 2, true);
        }

        // Top vent
        hull() {
            translate([0, 0, M_JOINT + 12.0]) rotate([-90, 0, 0]) cylinder(h=100, d=10.0);
            translate([0, 0, M_FORE - 12.0]) rotate([-90, 0, 0]) cylinder(h=100, d=10.0);
        }
    }
}

if (DRAW_PLATE) color("silver") {
    $fn=80;
    BOLT_D = 4.5;
    HEAD_D = 7.8;

    difference() {
        intersection() 
        {
            hull() {
                translate([0, 0, M_PLATE + W_PLATE/2]) rotate([-90, 0, 0]) cylinder(h=100, d=W_PLATE);
                translate([0, 0, M_PLATE + DZ_PLATE - W_PLATE/2]) rotate([-90, 0, 0]) cylinder(h=100, d=W_PLATE);
            }
            intersection() {
                translate([0, 0, M_PLATE]) cylinder(h=DZ_PLATE, d=D_OUTER);
                translate([-50, D_FORE_INNER/2 - DY_FLAT, M_PLATE]) cube(size=[100, 100, DZ_PLATE]);
            }
        }
        translate([0, 0, M_PORT]) rotate([-90, 0, 0]) cylinder(h=100, d=11.0);
        translate([0, 0, M_BOLT]) rotate([-90, 0, 0]) cylinder(h=100, d=BOLT_D);
        translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=100, d=8.0);
    }
}

*translate([0, 0, M_MC + DZ_SPEAKER]) {
    color("red") battery(D_AFT_INNER);
}
