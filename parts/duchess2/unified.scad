use <../commonUnified.scad>
use <../shapes.scad>
use <../inset.scad>
include <dim.scad>

$fn = 80;
DRAW_AFT = true;
DRAW_FORE = true;
DRAW_BRIDGE = false;
DRAW_RING = false;
DRAW_CHAMBER = false;
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

module tubeAtY(d, h) {
    translate([0, D_VENT_INNER/2 - D_TUBE/2, 0]) cylinder(h=h, d=d);
}

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

module ledPylon2()
{
    W = 17;
    FILM = 12.2;
    FILM_THICKNESS = 0.8;

    difference() {
        translate([-W/2, -D_INNER/2, M_AFT_FRONT - 5])
            cube(size=[W, D_INNER/2 + 1, 5]);
        translate([-FILM/2, -D_INNER/2, M_AFT_FRONT - FILM_THICKNESS])
            cube(size=[FILM, 100, 100]);
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
            baffleMCBattery(D_INNER, N_BATT_BAFFLES, DZ_BAFFLE, bridgeStyle=2, thermalRelief=3, thermalReliefOffset=2);
        }
    }
    translate([0, 0, M_JOINT]) rotate([0, 0, 0])
        keyJoint(JOINT, D_INNER, D_INNER - KEYJOINT_T, false);

}

module windows(start)
{
    for(y=[start:2]) {
        translate([-50, -D_INNER/2 + y * 6 - 1, 0]) {
            if (y==0)
                rotate([-45, 0, 0]) translate([0, 0, -6]) cube(size=[100, 6, 6]);
            else
                rotate([-45, 0, 0]) cube(size=[100, 3, 3]);
        }
    }
}

module inner()
{
    translate([0, 0, M_START]) cylinder(h=DZ_AFT, d=D_INNER);
    translate([0, 0, M_AFT_FRONT]) cylinder(h=200, d=D_VENT_INNER);
}

if (DRAW_FORE) {
    difference() {
        union() {
            translate([0, 0, M_JOINT]) {
                powerPortRing(
                    D_INNER, T, 
                    DZ_POWER_RING, DZ_POWER_RING/2, 
                    yTop=D_INNER /2 - TOP_FLATTEN, topNut=true);
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
                //switchHolder(D_INNER, M_SWITCH - M_SWITCH_START, 0, 10.5, false);
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

            // chamber base & holder
            translate([0, 0, M_AFT_FRONT]) cylinder(h=DZ_CHAMBER_BASE, d=D_VENT_INNER);

            intersection() {
                inner();
                ledPylon2();
            }
        }
        bottomDotstar();

        translate([0, 0, M_SWITCH_START]) scale([1.01, 1.01, 1.01]) {
            switchHolder(D_INNER, M_SWITCH - M_SWITCH_START, 0, 10.5, false);
        }

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

        // Access for dotstar
        FILM_WIDTH = 14;
        translate([-FILM_WIDTH/2, 5, M_AFT_FRONT - 1])
            cube(size=[FILM_WIDTH, 100, 1]);

        // bolts, going forward
        // Access to tubes for wiring
        translate([0, 0, M_AFT_FRONT - 1]) {
            chamberBolt(A_BOLT_0, D_ROD, 100.0);
            chamberBolt(A_BOLT_1, D_ROD, 100.0);
        }
        translate([0, 0, M_AFT_FRONT - 3.0]) {
            rotate([0, 0, A_BOLT_0 + ANGLE_OFFSET]) tubeAtY(D_TUBE_INNER, 100);
            rotate([0, 0, A_BOLT_1 + ANGLE_OFFSET]) tubeAtY(D_TUBE_INNER, 100);
        }
        // Was: 5mm thick - 2mm setback = 3mm in front
        translate([0, 0, M_AFT_FRONT + 3.0]) {
            rotate([0, 0, A_BOLT_0 + ANGLE_OFFSET]) tubeAtY(D_TUBE, 100);
            rotate([0, 0, A_BOLT_1 + ANGLE_OFFSET]) tubeAtY(D_TUBE, 100);
        }
        translate([-DOTSTAR_XZ/2, -DOTSTAR_XZ/2, M_AFT_FRONT]) {
            cube(size=[DOTSTAR_XZ, DOTSTAR_XZ, 10]);
        }
        // slide for led
        translate([-DOTSTAR_XZ/2, -DOTSTAR_XZ/2, M_AFT_FRONT]) {
            cube(size=[DOTSTAR_XZ, 100, 2.6]);
        }

    }
}

if (DRAW_BRIDGE) {
    translate([0, 0, M_SWITCH_START]) {
        switchHolder(D_INNER, M_SWITCH - M_SWITCH_START, 0, 10.5, false);
    }
}

module chamberRing()
{
    difference() {
        cylinder(h=CHAMBER_RING_DZ, d=D_VENT_INNER);
        translate([0, 0, -EPS]) {
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

module tubeCap(t)
{
    translate([0, D_VENT_INNER/2 - D_ROD_NUT/2, 0]) 
        cylinder(h=100, d=D_ROD_LOOSE);
    rotate([0, 0, ANGLE_OFFSET]) {
        translate([0, D_VENT_INNER/2 - D_TUBE/2, 0]) {
            cylinder(h=t, d=D_TUBE);
            translate([0, 0, t])
                cylinder(h=100, d=D_TUBE_INNER);
        }
    }
}

if (DRAW_CHAMBER) {
    *translate([0, 0, M_RING0_CENTER - CHAMBER_RING_DZ/2]) chamberRing();
    *translate([0, 0, M_RING1_CENTER - CHAMBER_RING_DZ/2]) chamberRing();

    // 3rd ring caps it.
    // translate([0, 0, M_RING2_CENTER - CHAMBER_RING_DZ/2]) chamberRing();

    T_LOWER = 4.0;
    T_HALF = 2.0;
    START = M_RING2_CENTER - CHAMBER_RING_DZ/2 + T_BRASS;
    W = 16;
    PCB_T = 1.6;
    PCB_W = 12.0;
    PCB_HOLE = 10.5;
    ANGLE = 40;

    // Brass plate, then plastic:
    *color("gold") translate([0, 0, M_RING2_CENTER - CHAMBER_RING_DZ/2]) {
        cylinder(h=T_BRASS, d=D_VENT_INNER);
    }

    difference() {
        intersection() {
            inner();
            translate([0, 0, START]) {
                difference() {
                    union() {
                        cylinder(h=T_LOWER, d=D_VENT_INNER);
                        rotate([0, 0, ANGLE]) {
                            translate([7, -W/2, 0]) cube(size=[100, W, M_FOOTER - START]);
                            mirror([-1, 0, 0]) translate([7, -W/2, 0]) cube(size=[100, W, M_FOOTER - START]);
                        }
                    }
                    rotate([0, 0, A_BOLT_0]) tubeCap(T_HALF);
                    rotate([0, 0, A_BOLT_1]) tubeCap(T_HALF);
                    cylinder(h=100, d=D_CRYSTAL_SPACE);
                }              
            }
        }    
        rotate([0, 0, ANGLE]) {
            translate([-50, -PCB_W/2, M_FOOTER - PCB_T]) cube(size=[100, PCB_W, 100]);
            translate([PCB_HOLE, 0, 0]) cylinder(h=1000, d=2.0);
            translate([-PCB_HOLE, 0, 0]) cylinder(h=1000, d=2.0);
        }
    }
    echo("DZ base-ring0", (M_RING0_CENTER - CHAMBER_RING_DZ/2) - (M_AFT_FRONT + DZ_CHAMBER_BASE));
    echo("DZ ring0-ring1", (M_RING1_CENTER - CHAMBER_RING_DZ/2) - (M_RING0_CENTER + CHAMBER_RING_DZ/2));
    echo("DZ ring1-coupler brass", (START - T_BRASS) - (M_RING1_CENTER + CHAMBER_RING_DZ/2));

    TUBE_START = M_AFT_FRONT + DZ_CHAMBER_BASE - TUBE_HOLDER_SETBACK;
    TUBE_END = START + T_HALF;    
    echo("DZ tube", TUBE_END - TUBE_START);

}

if (DRAW_SPACER) {
    intersection() {
        cylinder(h=1000, d=D_VENT_INNER);
        translate([0, 0, M_RING0_CENTER - CHAMBER_RING_DZ/2]) {
            difference() {
                rotate([0, 0, A_CHAMBER_CUT_0 + 29]) polygonXY(h=CHAMBER_RING_DZ, points=[
                    [0, 0], [-30, 30], [30, 30]
                ]);
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
*color("blue") translate([0, 0, M_FOOTER - 1.6]) cylinder(h=1.6, d=10);

*color("blue")  rotate([0, 0, 45-5]) translate([-1, -20, M_AFT_FRONT])
    cube(size=[2, 40, 1]);
*color("blue")  rotate([0, 0, -45-5]) translate([-1, -20, M_AFT_FRONT])
    cube(size=[2, 40, 1]);
