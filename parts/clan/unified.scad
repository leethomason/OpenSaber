//include <apprenticeV4.scad>
include <ventedBlack.scad>

use <../commonUnified.scad>
use <../shapes.scad>

DRAW_AFT = true;
DRAW_FORE = false;
DRAW_PLATE = false;
DRAW_PLATE_BASE = false;

EPS = 0.01;
$fn = 80;

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
M_BOLT_START = M_BOLT - 4.0;
M_SWITCH_START = M_BOLT + 4.0;
M_COUPLER_START = HAS_COUPLER ? M_SWITCH + 6.5 : M_SWITCH + 6.0;    // the non-coupler case is a hack

DZ_PORT_SECTION = M_BOLT_START - M_JOINT;
DZ_BOLT_SECTION = M_SWITCH_START - M_BOLT_START;
DZ_SWITCH_SECTION = M_COUPLER_START - M_SWITCH_START;
DZ_COUPLER = M_HEAT_SINK - M_COUPLER_START;

if (DRAW_AFT) {
    union() {
        translate([0, 0, M_MC]) {
            H_SPKR = 3.2;
            OFFSET = 2.0;
            if (D_AFT_INNER >= 29.5) {
                speakerHolder(D_AFT_INNER, DZ_SPEAKER, OFFSET, "std28");
            }
            else {
                difference() {
                    translate([0, 0, OFFSET + H_SPKR])
                        tube(h=DZ_SPEAKER - (OFFSET + H_SPKR), do=D_AFT_INNER, di=D_AFT_INNER - 5.0);
                    translate([-50, -50, 0])
                        cube(size=[100, 50 - 7.0, DZ_SPEAKER]);
                }
            }
        }
        translate([0, 0, M_MC + DZ_SPEAKER]) {
            baffleMCBattery(D_AFT_INNER, 
                            N_BAFFLES,
                            DZ_BAFFLE,
                            bridgeStyle=2);
        }
    }
    color("olive") translate([0, 0, M_JOINT]) {
        D = HAS_FORE_AFT ? D_AFT_INNER : D_FORE_INNER;
        keyJoint(KEYJOINT, D, D - KEYJOINT_T, false);

        if (HAS_KEYJOINT_PILLAR) {
            DZ = 2.0;
            intersection() {
                translate([0, 0, -DZ])
                    cylinder(h=100, d=D_AFT_INNER);
                union() {
                    translate([D/2 - 3.0, -D_AFT_INNER/2, -DZ])
                        cube(size=[4, D_AFT_INNER, DZ]);
                    mirror([-1, 0, 0]) translate([D/2 - 3.0, -D_AFT_INNER/2, -DZ])
                        cube(size=[4, D_AFT_INNER, DZ]);
                }
            }
        }
    }
}

if (DRAW_FORE) {
    difference() {
        union() {
            if (HAS_FORE_AFT) translate([0, 0, M_JOINT]) {
                tube(h=M_FORE - M_JOINT, do=D_AFT_INNER, di=D_FORE_INNER - T);
            }
            color("purple") translate([0, 0, M_JOINT]) {
                powerPortRing(D_FORE_INNER, T, DZ_PORT_SECTION, 
                            M_PORT - M_JOINT,
                            counter=false,
                            portSupportToBack=true);
            }
            color("orange") translate([0, 0, M_BOLT_START]) {
                boltRing(D_FORE_INNER, T, DZ_BOLT_SECTION, M_BOLT - M_BOLT_START);
            }            
            color("olive") translate([0, 0, M_SWITCH_START]) {
                DY_TO_PLATE_BASE = D_FORE_INNER / 2 - DY_FLAT;

                tactileRing(
                    D_FORE_INNER, T, 
                    DZ_SWITCH_SECTION, 
                    M_SWITCH - M_SWITCH_START, 
                    HAS_PIN_IGNITION, 
                    DY_TO_PLATE_BASE);
            }
            if (HAS_COUPLER) {
                translate([0, 0, M_COUPLER_START]) {
                    emitterCouplerRing(D_FORE_INNER, T, DZ_COUPLER);
                }
            }
        }
        // Flat top
        translate([-50, D_FORE_INNER/2 - DY_FLAT, M_FORE]) {
            cube(size=[100, 10, M_HEAT_SINK - M_FORE]);
        }

        // flat bottom
        translate([-50, -D_FORE_INNER/2 - 10, M_JOINT]) {
            cube(size=[100, 10.5, 200]);
        }

        // Bottom access
        hull() {
            translate([0, -D_FORE_INNER/2 + T, M_PORT + 1.0]) 
                rotate([90, 0, 0]) cylinder(h=20, d=12.0);
            translate([0, -D_FORE_INNER/2 + T, M_SWITCH - 2.0]) 
                rotate([90, 0, 0]) cylinder(h=20, d=12.0);
        }
        translate([0, 0, M_JOINT]) {
            D = HAS_FORE_AFT ? D_AFT_INNER : D_FORE_INNER;
            keyJoint(KEYJOINT, D, D - KEYJOINT_T, true);
        }

        // Side vents
        if (HAS_SIDE_VENTS) intersection() {
            translate([0, 0, M_JOINT]) tube(h=100, do=100, di=D_FORE_INNER - T - 0.1);
            for(i=[0:3])
                translate([0, 0, M_JOINT + 15 + i*8]) capsule(70, 110, 2, true);
        }

        // Top vent (joint ring)
        if (HAS_FORE_AFT) hull() {
            translate([0, 0, M_JOINT + 12.0]) rotate([-90, 0, 0]) cylinder(h=100, d=10.0);
            translate([0, 0, M_FORE - 12.0]) rotate([-90, 0, 0]) cylinder(h=100, d=10.0);
        }

        // Top vent (inner)
        W = 12;
        translate([-W/2, 0, M_FORE ]) cube(size=[W, 100, M_PORT - 8.0 - M_FORE]);
    }
}

if (DRAW_PLATE || DRAW_PLATE_BASE) color("silver") {
    $fn=160;
    BOLT_D = 4.5;

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
        if (DRAW_PLATE) {
            translate([0, 0, M_PORT]) rotate([-90, 0, 0]) cylinder(h=100, d=11.0);
            translate([0, 0, M_BOLT]) rotate([-90, 0, 0]) cylinder(h=100, d=BOLT_D);
            translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=100, d=HAS_PIN_IGNITION ? 3.6 : 8.0);
        }
    }
}

*translate([0, 0, M_MC + DZ_SPEAKER]) {
    color("red") battery(D_AFT_INNER);
}

*translate([0, 0, DZ_TOTAL]) color("red") cylinder(h=1.0, d=D_FORE_INNER);

echo("DZ_PORT, DZ_BOLT, DZ_SWITCH=", M_PORT - M_PLATE, M_BOLT - M_PLATE, M_SWITCH - M_PLATE);
echo("H_PLATE=", D_OUTER/2 - (D_FORE_INNER/2 - DY_FLAT));