use <../commonUnified.scad>
use <../shapes.scad>
use <../inset.scad>
include <dim.scad>
include <dimCoupler.scad>

$fn = 80;
DRAW_AFT = false;
DRAW_FORE = true;

EPS = 0.01;
ESP2 = 2 * EPS;

PLATE_TRIM = 1.0;
JOINT = 8;
T = 4;
TOP_FLATTEN = 3.0;

DOTSTAR_XZ = 5.6;
DOTSTAR_PITCH = 7;
DOTSTAR_STRIP_XZ = 12.4;
DOTSTAR_Z = 73.0 - 21.0;
KEYJOINT_T = 5.0;
COUPLER_ANGLE = -10.0;

module ring(dz)
{
    difference() {
        cylinder(h=dz, d=D_RING);
 
        translate([R_TUBE, 0, 0]) cylinder(h=dz, d=D_TUBE_INNER);
        translate([R_TUBE, 0, dz/2]) cylinder(h=dz, d=D_TUBE_OUTER);
 
        translate([-R_TUBE, 0, 0]) cylinder(h=dz, d=D_TUBE_INNER);
        translate([-R_TUBE, 0, dz/2]) cylinder(h=dz, d=D_TUBE_OUTER);

        scale([1.4, 1, 1]) hull() {
            translate([0, R_TUBE, 0])  cylinder(h=dz, d=6.0);
            translate([0, -R_TUBE, 0]) cylinder(h=dz, d=6.0);
        }
    }
}

module bottomDotstar()
{
    H0 = 0.8;
    H1 = 8;
    D = 10;
    OFFSETY = 0.1;

    hull() {
        for(i=[0:3]) {
            translate([-DOTSTAR_XZ/2, -D_INNER/2, DOTSTAR_Z + i * DOTSTAR_PITCH - DOTSTAR_XZ/2])
                cube(size=[DOTSTAR_XZ, 8, DOTSTAR_XZ]);
        }
    }
    EXTRAZ = 5.0;
    hull() {
        translate([-DOTSTAR_STRIP_XZ/2, -D_INNER/2 + T/2 + OFFSETY, DOTSTAR_Z - 3*DOTSTAR_XZ/2 + 2 - EXTRAZ])
            cube(size=[DOTSTAR_STRIP_XZ, H0, DOTSTAR_PITCH * 5 + EXTRAZ]);
        translate([-(DOTSTAR_STRIP_XZ-D)/2, -D_INNER/2 + T/2 + OFFSETY, DOTSTAR_Z - 3*DOTSTAR_XZ/2 + 2 - EXTRAZ])
            cube(size=[DOTSTAR_STRIP_XZ - D, H1, DOTSTAR_PITCH * 5 + EXTRAZ]);
    }
}

if (DRAW_AFT) {
    translate([0, 0, M_START]) {
        speakerHolder(D_INNER, DZ_SPKR, 3.0, "std28");
    }

    difference() {
        translate([0, 0, M_MC_BATTERY]) {
            baffleMCBattery(D_INNER, N_BATT_BAFFLES, DZ_BAFFLE, bridgeStyle=2);
        }
    }
    translate([0, 0, M_JOINT]) rotate([0, 0, 0])
        keyJoint(JOINT, D_INNER, D_INNER - KEYJOINT_T, false);

}

module windows(start)
{
    for(y=[start:3]) {
        translate([-50, -D_INNER/2 + y * 6 - 1, 0]) {
            TH = 3.0;
            TW = 2.0;
            //polygonYZ(h=100, points=[
            //    [0, 0], [0, TW], [TH, TW], [TH+TW/2, TW/2], [TH,0]
            //]);
            if (y==0)
                rotate([-45, 0, 0]) translate([0, 0, -6]) cube(size=[100, 4, 8]);
            else
                rotate([-45, 0, 0]) cube(size=[100, 4, 2]);
        }
    }
}

if (DRAW_FORE) {
    difference() {
        union() {
            translate([0, 0, M_JOINT]) {
                powerPortRing(D_INNER, T, DZ_POWER_RING, DZ_POWER_RING/2, addY=TOP_FLATTEN, dyPort=5.0);
            }

            translate([0, 0, M_BOLT_START]) {
                boltRing(D_INNER, T, DZ_BOLT, DZ_BOLT/2);
            }

            translate([0, 0, M_SWITCH_START]) {
                difference() {
                    W = 22;
                    tube(h=M_AFT_FRONT - M_SWITCH_START, do=D_INNER, di=D_INNER - T);
                    translate([-W/2, 0, 0]) cube(size=[W, 100, 100]);
                }
                switchHolder(D_INNER, M_SWITCH - M_SWITCH_START, 0, 10.5);
            }
            // Pillars to toughen front.
            PILLAR = HEAD_HOLDER_SETBACK;
            PILLAR_START = M_BOLT + DZ_BOLT/2;
            translate([0, 0, PILLAR_START]) {
                DZS = M_AFT_FRONT - PILLAR_START;
                intersection() {
                    cylinder(h=DZS, d=D_INNER);
                    union() {
                        P = [[4, D_INNER/2], [D_INNER/2, D_INNER/2], [D_INNER * 0.6, -D_INNER/2]];
                        polygonXY(h=DZS, points=P);
                        mirror([-1, 0, 0]) polygonXY(h=DZS, points=P);
                    }
                }
            }
            intersection() {
                cylinder(h=1000, d=D_INNER);
                union() {
                    translate([6, -100, M_AFT_FRONT - HEAD_HOLDER_SETBACK])
                        cube(size=[100, 100 + 4.0, HEAD_HOLDER_SETBACK]);
                    mirror([-1, 0, 0]) translate([6, -100, M_AFT_FRONT - HEAD_HOLDER_SETBACK])
                        cube(size=[100, 100 + 4.0, HEAD_HOLDER_SETBACK]);
                }
            }
            // toughen up the sides
            FORE_TRIM = DZ_POWER_RING;
            difference() {
                THICK = 5.0;
                translate([0, 0, M_JOINT]) intersection() {
                    cylinder(d=D_INNER, h=FORE_TRIM);
                    union() {
                        translate([D_INNER/2 - THICK, -50, 0]) cube(size=[10, 100, 100]);
                        mirror([-1, 0, 0]) translate([D_INNER/2 - THICK, -50, 0]) cube(size=[10, 100, 100]);
                    }
                }
                cylinder(h=100, d=D_INNER - 8);
            }

            // thicken the bottom for dotstar
            intersection() 
            {
                DY = 4.5;
                translate([0, 0, M_JOINT]) cylinder(h=FORE_TRIM, d=D_INNER);
                translate([-50, -D_INNER/2, M_JOINT]) cube(size=[100, DY, FORE_TRIM]);
            }
        }
        bottomDotstar();

        translate([0, 0, M_PORT + 6.5]) windows(0);
        translate([0, 0, M_BOLT + 7.0]) windows(1);

        // Flat area for switch plate.
        translate([-50, D_INNER/2 - TOP_FLATTEN, M_JOINT])
            cube(size=[100, 10, 100]);
        
        translate([0, 0, M_JOINT]) rotate([0, 0, 0])
            keyJoint(JOINT, D_INNER, D_INNER - KEYJOINT_T, true);

        // Flat bottom
        translate([-50, -D_INNER/2 - EPS, 0]) cube(size=[100, 1, 500]);

        // Access to port
        translate([0, 0, M_PORT]) rotate([90, 0, 0]) cylinder(h=100, d=12);

        // Access to bottom
        W_ACCESS = 12.0;
        translate([-W_ACCESS/2, -D_INNER/2, M_JOINT]) cube(size=[W_ACCESS, 2.5, 22]);

        // bolt holder
        translate([10.0, -DY_HEAD_HOLDER/2, M_AFT_FRONT-HEAD_HOLDER_SETBACK-DZ_HEAD_HOLDER])
            cube(size=[100, DY_HEAD_HOLDER, DZ_HEAD_HOLDER]);
        mirror([-1, 0, 0]) translate([10.0, -DY_HEAD_HOLDER/2, M_AFT_FRONT-HEAD_HOLDER_SETBACK-DZ_HEAD_HOLDER])
            cube(size=[100, DY_HEAD_HOLDER, DZ_HEAD_HOLDER]);

        // bolts
        rotate([0, 0, A_BOLT_0])
            translate([0, D_VENT_INNER/2-D_BOLT_CAP/2, M_AFT_FRONT - HEAD_HOLDER_SETBACK - 1.0])
                cylinder(h=30.0, d=D_ROD + 0.2);
        rotate([0, 0, A_BOLT_1])
            translate([0, D_VENT_INNER/2-D_BOLT_CAP/2, M_AFT_FRONT - HEAD_HOLDER_SETBACK - 1.0])
                cylinder(h=30.0, d=D_ROD + 0.2);
    }
}

*color("blue") translate([0, 0, M_START-1]) cylinder(h=1, d=D_OUTER);
*color("blue") translate([0, 0, M_AFT_FRONT]) cylinder(h=1, d=D_OUTER);
*color("blue") translate([0, 0, M_AFT_FRONT]) cylinder(h=1, d=D_OUTER);

*color("gold") {
    rotate([0, 0, A_BOLT_0])
        translate([0, D_VENT_INNER/2-D_BOLT_CAP/2, M_AFT_FRONT])
            cylinder(h=30.0, d=D_ROD);
    rotate([0, 0, A_BOLT_1])
        translate([0, D_VENT_INNER/2-D_BOLT_CAP/2, M_AFT_FRONT])
            cylinder(h=30.0, d=D_ROD);
}