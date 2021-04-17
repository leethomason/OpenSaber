include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>

$fn = 60;

DRAW_AFT = false;
DRAW_FORE = true;

KEYJOINT = 8;
KEYJOINT_T = 4.5;
EPS = 0.01;

M_FORE = M_MC + (N_BAFFLES * 2 + 1) * DZ_BUTTRESS;

if (DRAW_AFT) {    
    difference() {
        union() {
            translate([0, 0, M_MC]) {
                for(i=[0:N_BAFFLES]) {
                    translate([0, 0, i*DZ_BUTTRESS*2])  {
                        //oneBaffle(D_INNER, DZ_BUTTRESS, 
                        //        battery=false,
                        //        mc=false,
                        //        bridge=2,
                        //        bottomRail=true
                        //);
                        cylinder(h=DZ_BUTTRESS, d=D_INNER);
                        if (i < N_BAFFLES)
                            bridgeAndRail(2, D_INNER, DZ_BUTTRESS, bottomRail=true);
                    }
                }
            }
        }
        translate([0, 0, M_MC]) {
            // Cut the top
            W = 10.0;
            translate([-W/2, 0, DZ_BUTTRESS*3]) 
                cube(size=[W, 100, 200]);

            // Cut the bottom
            BW = 12.0;
            translate([-BW/2, -100, 0]) cube(size=[BW, 100, 200]);
        }
        translate([-MC_X/2, -MC_Y/2, M_MC]) 
            cube(size=[MC_X, MC_Y, MC_Z]);
        translate([0, 0, M_BATTERY]) 
            cylinder(h=DZ_BATTERY, d=D_BATTERY);
    }
}

if (DRAW_FORE)
{
    PORT_DX = 9.00;
    PORT_DY = 14.35 + 0.5;
    PORT_DZ = 10.70;
    PORT_CENTER = 6.60;
    PORT_PAST = PORT_DZ - PORT_CENTER;

    DZ_SECTION = M_END - M_FORE;

    YATX_PORT = yAtX(PORT_DX/2, D_INNER/2);
    PORT_Y = YATX_PORT - PORT_DY;
    T_HOLDER = 3;
    T_INNER = 2.0;

    PCB_DX = 16.8;
    PCB_DX_WIDE = 18;
    PCB_DZ = 16.8;
    PCB_OFFSET = 25.4 * 0.05;
    PCB_BUTTON = 6.7;
    PCB_BASE = yAtX(PCB_BUTTON/2, D_INNER/2) - 6.0 - 1.6 - 0.5;
    PCB_HOLE_X = 6.35;
    PCB_HOLE_Z = 6.35;

    DZ_BOLT = 13.0;
    D_BOLT = 3.0;

    difference() {
        union() {
            translate([0, 0, M_FORE]) {
                boltRing(D_INNER, T, DZ_BOLT, M_BOLT - M_FORE, 
                    bolt_d = D_BOLT + 0.2,
                    nut_w = 5.50, 
                    nut_y = 2.40,
                    dy_port = 10.0);
                translate([0, 0, DZ_BOLT]) 
                    tube(h=DZ_SECTION - DZ_BOLT, do=D_INNER, di=D_INNER - T);
            }

            // Joint reinforce
            intersection() {
                translate([0, 0, M_FORE]) cylinder(h=DZ_SECTION, d=D_INNER);
                union() {
                    JT = 5.0;
                    translate([D_INNER/2 - JT, -50, M_FORE]) cube(size=[JT, 100, KEYJOINT + 1]);
                    mirror([-1, 0, 0]) translate([D_INNER/2 - JT, -50, M_FORE]) cube(size=[JT, 100, KEYJOINT + 1]);
                }
            }

            // Port holder.
            intersection() {
                translate([0, 0, M_FORE]) cylinder(h=DZ_SECTION, d=D_INNER);
                difference() {
                    union() {
                        OFFSET = 4.0;
                        translate([2, -D_INNER/2, M_PORT - PORT_CENTER - OFFSET])   
                            cube(size=[PORT_DX/2 + T_HOLDER - 2, D_INNER/2 + 1, PORT_DZ + OFFSET]);
                        mirror([-1, 0, 0]) translate([0, -D_INNER/2, M_PORT - PORT_CENTER])   
                            cube(size=[PORT_DX/2 + T_HOLDER, D_INNER/2 + 1, PORT_DZ]);
                    }
                    translate([-PORT_DX/2 + T_INNER, -D_INNER/2, M_PORT - PORT_CENTER])   
                        cube(size=[PORT_DX + -T_INNER*2, 20, PORT_DZ]);
                }
            }
        }
        // Cut port out.
        translate([-PORT_DX/2, PORT_Y, M_PORT - PORT_CENTER]) 
            cube(size=[PORT_DX, 20, PORT_DZ]);

        // Cut PCB out
        translate([-PCB_DX_WIDE/2, 0, M_SWITCH - PCB_DZ/2 - PCB_OFFSET])
            cube(size=[PCB_DX_WIDE, 20, PCB_DZ]);
        translate([0, 0, M_FORE - EPS])
            keyJoint(KEYJOINT + EPS, D_INNER, D_INNER - KEYJOINT_T, true);

        // Bottom access
        hull() {
            translate([0, 0, M_FORE + 14]) rotate([90, 0, 0]) cylinder(h=20, d=16);
            translate([0, 0, M_FORE + 25]) rotate([90, 0, 0]) cylinder(h=20, d=16);
        }
        // side access
        //translate([0, 0, M_FORE + 32]) triCapsule(70, 110, 2, true);
        //translate([0, 0, M_FORE + 22]) triCapsule(70, 110, 2, true);

        // Flat bottom
        translate([-50, -D_INNER/2 -1, M_FORE]) cube(size=[100, 1.5, 100]);
    }
    // PCB holder
    intersection() {
        translate([0, 0, M_FORE]) cylinder(h=DZ_SECTION, d=D_INNER);
        union() {
            translate([PCB_HOLE_X, PCB_BASE, M_SWITCH + PCB_HOLE_Z - PCB_OFFSET]) pcbButtress();
            translate([PCB_HOLE_X, PCB_BASE, M_SWITCH - PCB_HOLE_Z - PCB_OFFSET]) pcbButtress();
            mirror([-1, 0, 0]) translate([PCB_HOLE_X, PCB_BASE, M_SWITCH + PCB_HOLE_Z - PCB_OFFSET]) pcbButtress();
            mirror([-1, 0, 0]) translate([PCB_HOLE_X, PCB_BASE, M_SWITCH - PCB_HOLE_Z - PCB_OFFSET]) pcbButtress();
        }
    }

    *color("plum") translate([-PORT_DX/2, PORT_Y, M_PORT - PORT_CENTER]) 
        cube(size=[PORT_DX, PORT_DY, PORT_DZ]);

}

//color("red") translate([-MC_X/2, -MC_Y/2, M_MC]) cube(size=[MC_X, MC_Y, MC_Z]);
//color("green") translate([0, 0, M_BATTERY]) cylinder(h=DZ_BATTERY, d=D_BATTERY);

//color("red") translate([0, 0, M_BOLT]) rotate([-90, 0, 0]) cylinder(h=10, d=2);
//color("red") translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=10, d=12);
//color("red") translate([0, 0, M_PORT]) rotate([-90, 0, 0]) cylinder(h=10, d=9);
