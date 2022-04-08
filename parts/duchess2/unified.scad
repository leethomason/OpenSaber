use <../commonUnified.scad>
use <../shapes.scad>
use <../inset.scad>
include <dim.scad>

$fn = 80;
DRAW_AFT = false;
DRAW_FORE = true;
DRAW_RING = false;
DRAW_CHAMBER = true;
DRAW_SPACER = false;
DRAW_BRASS = false;

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

module chamberRod(dRod) {
    translate([0, D_VENT_INNER/2 - D_ROD_NUT/2]) cylinder(h=100, d=dRod);
    rotate([0, 0, ANGLE_OFFSET]) hull() {
        translate([0, D_VENT_INNER/2 - D_TUBE/2, 0]) cylinder(h=100, d=D_TUBE);
        translate([0, 30, 0]) cylinder(h=100, d=D_TUBE);
    }
}

module chamberBolt(angle, d, h)
{
    rotate([0, 0, angle])
        translate([0, D_VENT_INNER/2 - D_ROD_NUT/2, 0])
            cylinder(h=h, d=d);
}

module ledPylon(dz)
{
    W = 20.0;
    W2 = 2.0;
    WB = 5.0;
    WB2 = 16.0;

    DOTSTAR = 5.0;
    FILM = 12.2;
    FILM_THICKNESS = 0.6;
    CLEARANCE = 1.0;
    BACK = 0.6 * dz;

    intersection() {
        translate([0, 0, -EPS]) cylinder(d=D_INNER, h=dz + 2 *EPS);
        difference() {
            translate([-W/2, -D_INNER/2, 0]) 
                cube(size=[W, D_INNER/2 + DOTSTAR/2, dz]);

            translate([0, 0, -EPS]) polygonXY(h=dz + EPS*2, points=[
                [-WB2/2, -D_INNER/2],
                [0, -DOTSTAR],
                [WB2/2, -D_INNER/2]
            ]);

            // Knock out dotstar.
            translate([-DOTSTAR/2, -DOTSTAR/2, BACK])
                cube(size=[DOTSTAR, 100, 10]);
            // Knock out film
            translate([-FILM/2, -DOTSTAR/2, BACK - FILM_THICKNESS])
                cube(size=[FILM, 100, FILM_THICKNESS]);

            // Space behind...
            translate([-W/2, DOTSTAR/2 - CLEARANCE, -10])
                cube(size=[W, DOTSTAR, 10 + BACK]);

            translate([DOTSTAR/2, -DOTSTAR/2, 0])
                cube(size=[100, 100, BACK]);
            mirror([-1, 0, 0]) translate([DOTSTAR/2, -DOTSTAR/2, 0])
                cube(size=[100, 100, BACK]);
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
        translate([0, 0, M_MC]) {
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
            if (y==0)
                rotate([-45, 0, 0]) translate([0, 0, -6]) cube(size=[100, 6, 6]);
            else
                rotate([-45, 0, 0]) cube(size=[100, 3, 3]);
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
            // Front attachment points for tubes & rods.
            intersection() {
                cylinder(h=1000, d=D_INNER);
                PDY = 7.5;
                X = 5.0;
                union() {
                    translate([X, -100, M_AFT_FRONT - HEAD_HOLDER_SETBACK])
                        cube(size=[100, 100 + PDY, HEAD_HOLDER_SETBACK]);
                    mirror([-1, 0, 0]) translate([X, -100, M_AFT_FRONT - HEAD_HOLDER_SETBACK])
                        cube(size=[100, 100 + PDY, HEAD_HOLDER_SETBACK]);
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
            DLED = 5;
            translate([0, 0, M_AFT_FRONT - DLED]) ledPylon(DLED);
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

        // bolts, going forward
        // Access to tubes for wiring
        translate([0, 0, M_AFT_FRONT - HEAD_HOLDER_SETBACK - EPS]) {
            chamberBolt(A_BOLT_0, D_ROD, 100.0);
            chamberBolt(A_BOLT_1, D_ROD, 100.0);
            chamberBolt(A_BOLT_0 + ANGLE_OFFSET, D_TUBE_INNER, 100);
            chamberBolt(A_BOLT_1 + ANGLE_OFFSET, D_TUBE_INNER, 100);
        }
        translate([0, 0, M_AFT_FRONT - TUBE_HOLDER_SETBACK]) {
            chamberBolt(A_BOLT_0 + ANGLE_OFFSET, D_TUBE, 100);
            chamberBolt(A_BOLT_1 + ANGLE_OFFSET, D_TUBE, 100);
        }
    }
}

module chamberRing()
{
    difference() {
        cylinder(h=CHAMBER_RING_DZ, d=D_VENT_INNER);
        translate([0, 0, -1]) {
            cylinder(h=10, d=D_CRYSTAL_SPACE);
            rotate([0, 0, A_BOLT_0]) chamberRod(D_ROD);
            rotate([0, 0, A_BOLT_1]) chamberRod(D_ROD);                
        }
    }
}

module chamberPlate()
{
    difference() {
        cylinder(h=T_BRASS, d=D_VENT_INNER);
        translate([0, 0, -1]) {
            cylinder(h=10, d=D_CRYSTAL_SPACE);
            rotate([0, 0, A_BOLT_0]) chamberRod(D_ROD_LOOSE);
            rotate([0, 0, A_BOLT_1]) chamberRod(D_ROD_LOOSE);                
            rotate([0, 0, A_CHAMBER_CUT_0]) translate([0, R_CHAMBER_CUT, 0]) cylinder(h=100, d=D_CHAMBER_CUT);
            rotate([0, 0, A_CHAMBER_CUT_1]) translate([0, R_CHAMBER_CUT, 0]) cylinder(h=100, d=D_CHAMBER_CUT);
            rotate([0, 0, A_CHAMBER_CUT_2]) translate([0, R_CHAMBER_CUT, 0]) cylinder(h=100, d=D_CHAMBER_CUT);
            rotate([0, 0, A_CHAMBER_CUT_0 + 180]) translate([0, R_CHAMBER_CUT, 0]) cylinder(h=100, d=D_CHAMBER_CUT);
            rotate([0, 0, A_CHAMBER_CUT_1 + 180]) translate([0, R_CHAMBER_CUT, 0]) cylinder(h=100, d=D_CHAMBER_CUT);
            rotate([0, 0, A_CHAMBER_CUT_2 + 180]) translate([0, R_CHAMBER_CUT, 0]) cylinder(h=100, d=D_CHAMBER_CUT);
        }
    }
}


if (DRAW_RING) {
    translate([0, 0, M_AFT_FRONT + 10]) {
        chamberRing();
    }
}

if (DRAW_CHAMBER) {
    translate([0, 0, M_RING0_CENTER - CHAMBER_RING_DZ/2]) chamberRing();
    translate([0, 0, M_RING1_CENTER - CHAMBER_RING_DZ/2]) chamberRing();
    translate([0, 0, M_RING2_CENTER - CHAMBER_RING_DZ/2]) chamberRing();

    T_LOWER = 3.0;
    T = 10.0;
    W = 16;
    
    PCB_T = 1.6;
    PCB_W = 12.0;
    PCB_HOLE = 10.5;

    intersection() {
        cylinder(h=200, d=D_VENT_INNER);
        difference() {
            union() {
                translate([0, 0, M_EMITTER_PLATE - T]) cylinder(h=T_LOWER, d=D_VENT_INNER);
                translate([-W/2, 8, M_EMITTER_PLATE - T]) cube(size=[W, 20, T]);
                mirror([0, -1, 0]) translate([-W/2, 8, M_EMITTER_PLATE - T]) cube(size=[W, 20, T]);
            }
            translate([0, 0, M_EMITTER_PLATE - T - 1]) {
                cylinder(h=100, d=D_CRYSTAL_SPACE);
                rotate([0, 0, A_BOLT_0]) chamberRod(D_ROD_LOOSE);
                rotate([0, 0, A_BOLT_1]) chamberRod(D_ROD_LOOSE);                
            }
            translate([-PCB_W/2, -50, M_EMITTER_PLATE - PCB_T]) cube(size=[PCB_W, 100, PCB_T]);
            translate([0, PCB_HOLE, 0]) cylinder(h=1000, d=2.2);
            translate([0, -PCB_HOLE, 0]) cylinder(h=1000, d=2.2);
        }
    }
}

if (DRAW_SPACER) {
    intersection() {
        cylinder(h=1000, d=D_VENT_INNER);
        translate([0, 0, M_RING0_CENTER - CHAMBER_RING_DZ/2]) {
            difference() {
                rotate([0, 0, -A_TILT + 4]) polygonXY(h=CHAMBER_RING_DZ, points=[
                    [0, 0], [-30, 30], [30, 30]
                ]);
                //translate([0, 0, -CHAMBER_RING_DZ /2]) chamberPlate();
                //cylinder(h=CHAMBER_RING_DZ, d=D_VENT_INNER);
                translate([0, 0, CHAMBER_RING_DZ - T_BRASS + EPS]) chamberPlate();
                translate([0, 0, 0]) chamberPlate();
                cylinder(h=100, d=D_CRYSTAL_SPACE);
            }
        }
    }
}

if (DRAW_BRASS) {
    translate([0, 0, M_RING0_CENTER - CHAMBER_RING_DZ/2])
        chamberPlate();
}

*color("blue") translate([0, 0, M_START-1]) cylinder(h=1, d=D_OUTER);
*color("blue") translate([0, 0, M_AFT_FRONT]) cylinder(h=1, d=D_OUTER);
*color("blue") translate([0, 0, M_AFT_FRONT]) cylinder(h=1, d=D_OUTER);
*color("blue") translate([0, 0, DZ_BODY]) cylinder(h=1, d=D_OUTER);
