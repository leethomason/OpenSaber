use <../commonUnified.scad>
use <../shapes.scad>
use <../inset.scad>
include <dim.scad>

$fn = 80;
DRAW_AFT = false;
DRAW_FORE = true;
DRAW_SWITCH_HOLDER = true;
//DRAW_RING0 = false;
//DRAW_RING1 = false;

EPS = 0.01;
ESP2 = 2 * EPS;

PLATE_TRIM = 1.0;
JOINT = 8;
T = 4;
TOP_FLATTEN = 3.0;

DOTSTAR_XZ = 5.6;
DOTSTAR_PITCH = 7;
DOTSTAR_STRIP_XZ = 12.4;
DOTSTAR_Z = 52.0;   // center of first, fixme
KEYJOINT_T = 5.0;

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

//if (DRAW_RING0) translate([0, 0, M_AFT_FRONT - DZ_RING0]) ring(DZ_RING0);
//if (DRAW_RING1) translate([0, 0, M_AFT_FRONT + 20.0]) mirror([0, 0, -1]) ring(DZ_RING1);

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

if (DRAW_SWITCH_HOLDER)
{
    HOLDER_Y = 10.0;
    HOLDER_LOWER_Y = 2.5;
    HOLDER_D = 8.0;         // fixme
    HOLDER_LOWER_D = 10.5;
    INNER_D = 4.0;          // fixme

    Y_OFFSET = 3.0;

    color("plum") translate([0, D_INNER/2 - TOP_FLATTEN - Y_OFFSET, M_SWITCH]) rotate([-90, 0, 0]) {
        difference() {
            union() {
                cylinder(h=HOLDER_Y, d=HOLDER_D);
                cylinder(h=HOLDER_LOWER_Y, d=HOLDER_LOWER_D);
            }
            cylinder(h=20, d=INNER_D);
        }
    }
}

if (DRAW_AFT) {
    translate([0, 0, M_START]) {
        speakerHolder(D_INNER, DZ_SPKR, 3.0, "std28");
    }

    difference() {
        translate([0, 0, M_MC_BATTERY]) {
            baffleMCBattery(D_INNER, N_BATT_BAFFLES, DZ_BAFFLE, bridgeStyle=2);
            *color("red") battery(D_INNER, "18650");
        }
    }
    translate([0, 0, M_JOINT]) rotate([0, 0, 0])
        keyJoint(JOINT, D_INNER, D_INNER - KEYJOINT_T, false);

}

if (DRAW_FORE) {

    difference() {
        union() {
            translate([0, 0, M_JOINT]) {
                powerPortRing(D_INNER, T, DZ_POWER_RING, DZ_POWER_RING/2, addY=TOP_FLATTEN, counter=false);
            }

            translate([0, 0, M_BOLT_START]) {
                boltRing(D_INNER, T, DZ_BOLT, DZ_BOLT/2);
            }

            translate([0, 0, M_SWITCH_START]) {
                difference() {
                    W = 22;
                    tube(h=M_AFT_THREAD_FRONT - DZ_RING0 - M_SWITCH_START, do=D_INNER, di=D_INNER - T);
                    translate([-W/2, 0, 0]) cube(size=[W, 100, 100]);
                }
                switchHolder(D_INNER, M_SWITCH - M_SWITCH_START, 0, 10.0);
            }
            // Pillars to toughen front.
            PILLAR = 5;
            //translate([0, 0, M_AFT_THREAD_FRONT - PILLAR - DZ_RING0]) {
            PILLAR_START = M_BOLT + DZ_BOLT/2;
            translate([0, 0, PILLAR_START]) {
                DZS = M_AFT_THREAD_FRONT - DZ_RING0 - PILLAR_START;
                intersection() {
                    cylinder(h=DZS, d=D_INNER);
                    union() {
                        P = [[4, D_INNER/2], [D_INNER/2, D_INNER/2], [D_INNER * 0.6, -D_INNER/2]];
                        polygonXY(h=DZS, points=P);
                        mirror([-1, 0, 0]) polygonXY(h=DZS, points=P);
                    }
                }
            }

            // toughen up the sides
            difference() {
                THICK = 5.0;
                translate([0, 0, M_JOINT]) intersection() {
                    cylinder(d=D_INNER, h=DZ_FORE_TRIM);
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
                translate([0, 0, M_JOINT]) cylinder(h=DZ_FORE_TRIM, d=D_INNER);
                translate([-50, -D_INNER/2, M_JOINT]) cube(size=[100, DY, DZ_FORE_TRIM]);
            }
        }
        bottomDotstar();

        // Flat area for switch plate.
        translate([-50, D_INNER/2 - TOP_FLATTEN, M_JOINT])
            cube(size=[100, 10, 100]);
        
        translate([0, 0, M_JOINT]) rotate([0, 0, 0])
            keyJoint(JOINT, D_INNER, D_INNER - KEYJOINT_T, true);

        // Flat bottom
        translate([-50, -D_INNER/2 - EPS, 0]) cube(size=[100, 1, 500]);
    }
}

*difference() {
    translate([-INSET_W/2, D_INNER/2, M_JOINT + PLATE_TRIM])
        roundedRect(size=[INSET_W, 1, M_AFT_THREAD_FRONT - M_JOINT - 2.0*PLATE_TRIM], r=1.6, up="y");
        
    translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=100, d=8.0);
    translate([0, 0, M_BOLT]) rotate([-90, 0, 0]) cylinder(h=100, d=4.2);
    translate([0, 0, M_PORT]) rotate([-90, 0, 0]) cylinder(h=100, d=11.0);
}

*color("blue") translate([0, 0, M_START-1]) cylinder(h=1, d=D_OUTER);
*color("blue") translate([0, 0, M_AFT_THREAD_FRONT]) cylinder(h=1, d=D_OUTER);
*color("blue") translate([0, 0, M_AFT_FRONT]) cylinder(h=1, d=D_OUTER);
