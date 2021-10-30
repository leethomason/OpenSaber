use <../commonUnified.scad>
use <../shapes.scad>
use <../inset.scad>
include <dim.scad>
include <dimCoupler.scad>
$fn = 80;
DRAW_AFT = false;
DRAW_FORE = true;
DRAW_SWITCH_HOLDER = false;
DRAW_COUPLER = false;    // debugging

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

if (DRAW_SWITCH_HOLDER)
{
    HOLDER_Y = 6.0;
    HOLDER_LOWER_Y = 2.5;
    HOLDER_D = 7.2;
    HOLDER_LOWER_D = 10.5;
    INNER_D = 4.1;

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
                    tube(h=M_AFT_FRONT - DZ_RING0 - M_SWITCH_START, do=D_INNER, di=D_INNER - T);
                    translate([-W/2, 0, 0]) cube(size=[W, 100, 100]);
                }
                switchHolder(D_INNER, M_SWITCH - M_SWITCH_START, 0, 10.5);
            }
            // Pillars to toughen front.
            PILLAR = 5;
            PILLAR_START = M_BOLT + DZ_BOLT/2;
            translate([0, 0, PILLAR_START]) {
                DZS = M_AFT_FRONT - DZ_RING0 - PILLAR_START;
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

            // Mounting for coupler
            intersection() {
                FRONT = M_AFT_FRONT - DZ_RING0;
                union() {
                    cylinder(h=FRONT, d=D_INNER);
                    intersection() {
                        cylinder(h=500.0, d=D_VENT_INNER);
                        rotate([0, 0, COUPLER_ANGLE])
                            translate([-50, 0, FRONT]) polygonYZ(h=100, points=[[-4,0], [-2,2], [2,2], [4,0]]);
                    }
                }
                SUPPORT = 6.0;
                union() {
                    translate([D_COUPLER_OUTER/2 - 4.0, -50, FRONT - SUPPORT]) 
                        cube(size=[100, 100, SUPPORT + DZ_FORE_SUPPORT]);
                    mirror([-1, 0, 0]) translate([D_COUPLER_OUTER/2 - 4.0, -50, FRONT - SUPPORT]) 
                        cube(size=[100, 100, SUPPORT + DZ_FORE_SUPPORT]);
                }
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

        // Holes for coupler mounting
        rotate([0, 0, COUPLER_ANGLE]) {
            translate([D_COUPLER_OUTER/2 - D_M2_HEAD/2, 0, M_COUPLER_START - 10]) cylinder(h=10, d=D_M2);
            translate([-(D_COUPLER_OUTER/2 - D_M2_HEAD/2), 0, M_COUPLER_START - 10]) cylinder(h=10, d=D_M2);
        }
    }
}

if (DRAW_COUPLER) {
    translate([0, 0, M_COUPLER_START]) {
        color("beige") cylinder(h=H_COUPLER_PCB, d=D_COUPLER_OUTER);
        translate([0, 0, H_COUPLER_PCB]) color("plum") cylinder(h=TOOTH_HEIGHT, d=D_COUPLER_OUTER);
        translate([0, 0, H_COUPLER_PCB + TOOTH_HEIGHT]) color("beige") cylinder(h=H_COUPLER_PCB, d=D_COUPLER_OUTER);
    }
}

*difference() {
    translate([-INSET_W/2, D_INNER/2, M_JOINT + PLATE_TRIM])
        roundedRect(size=[INSET_W, 1, M_AFT_FRONT - M_JOINT - 2.0*PLATE_TRIM], r=1.6, up="y");
        
    translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=100, d=8.0);
    translate([0, 0, M_BOLT]) rotate([-90, 0, 0]) cylinder(h=100, d=4.2);
    translate([0, 0, M_PORT]) rotate([-90, 0, 0]) cylinder(h=100, d=11.0);
}

*color("blue") translate([0, 0, M_START-1]) cylinder(h=1, d=D_OUTER);
*color("blue") translate([0, 0, M_AFT_FRONT]) cylinder(h=1, d=D_OUTER);
*color("blue") translate([0, 0, M_AFT_FRONT]) cylinder(h=1, d=D_OUTER);
