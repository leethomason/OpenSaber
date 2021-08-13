include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>

$fn = 60;

DRAW_AFT = true;
DRAW_FORE = false;

KEYJOINT = 8;
KEYJOINT_T = 4.5;
EPS = 0.01;

TACTILE_X = 12 + 0.2; // space for contacts
TACTILE_Z = 12 + 0.2;
TACTILE_DY = 6.5;

M_FORE = M_MC + (N_BAFFLES * 2 + 1) * DZ_BUTTRESS;

module bridge(dz)
{
    intersection()
    {
        cylinder(h=400, d=D_INNER);
        union() {
            polygonXY(dz, [
                [-D_INNER/2, -D_INNER/2],
                [-D_INNER/2, D_INNER/2],
                [0, D_INNER/2]
            ]);

            polygonXY(dz, [
                [D_INNER/2, -D_INNER/2],
                [D_INNER/2, D_INNER/2],
                [0, D_INNER/2]
            ]);
        }
    }
}

if (DRAW_AFT) {    
    difference() {
        union() {
            translate([0, 0, M_MC]) {
                for(i=[0:N_BAFFLES]) {
                    translate([0, 0, i*DZ_BUTTRESS*2])  {
                        cylinder(h=DZ_BUTTRESS, d=D_INNER);
                        if (i < N_BAFFLES) {
                            bottomRail = ((i+2)%4) != 0;
                            bridgeAndRail(2, D_INNER, DZ_BUTTRESS, bottomRail=bottomRail);
                        }
                    }
                }
            }
        }
        translate([0, 0, M_MC]) {
            // Cut the top
            W = 10.0;
            translate([-W/2, 0, 0]) //DZ_BUTTRESS*3]) 
                cube(size=[W, 100, 200]);

            // Cut the bottom
            BW = 12.0;
            translate([-BW/2, -100, DZ_BUTTRESS*3]) cube(size=[BW, 100, 200]);
        }
        translate([-MC_X/2, -2, M_MC]) 
            cube(size=[MC_X, MC_Y, MC_Z + 10]);
        translate([0, -0.5 * (D_INNER - D_BATTERY) / 2.0, M_BATTERY]) 
            cylinder(h=DZ_BATTERY, d=D_BATTERY);
    }
    translate([0, 0, M_FORE - EPS])
        keyJoint(KEYJOINT + EPS, D_INNER, D_INNER - KEYJOINT_T, false);
}

if (DRAW_FORE)
{
    PORT_DX = 9.00;
    PORT_DY = 14.35 + 0.5;
    PORT_DZ = 10.70;
    PORT_CENTER = 6.5;  // from the back

    DZ_SECTION = M_END - M_FORE;

    YATX_PORT = yAtX(PORT_DX/2, D_INNER/2);
    PORT_Y = YATX_PORT - PORT_DY;
    T_HOLDER = 3;
    T_INNER = 2.0;

    PCB_DX = 16.8;
    PCB_DX_WIDE = 18;
    PCB_DZ = 14.3;
    PCB_BUTTON = 6.7;
    PCB_BASE = yAtX(PCB_BUTTON/2, D_INNER/2) - 6.0 - 1.6 - 0.5;
    PCB_HOLE_X = 6.35;
    PCB_HOLE_Z = 5.08;

    DZ_BOLT = 13.0;
    D_BOLT = 3.0;
    BRIDGE = 2.0;

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
                ACCESS = 5.0;
                difference() {
                    translate([-PORT_DX/2 - T_INNER, -D_INNER/2, M_PORT - PORT_CENTER])   
                        cube(size=[PORT_DX + T_INNER*2, 50, PORT_DZ]);
                    // LED wire access
                    translate([-ACCESS/2, -D_INNER/2, M_PORT - PORT_CENTER])   
                        cube(size=[ACCESS, 50, PORT_DZ]);
                }                
            }

            // Tactile holder
            translate([0, 0, M_SWITCH - TACTILE_Z/2]) bridge(BRIDGE);
            translate([0, 0, M_SWITCH + TACTILE_Z/2 - BRIDGE]) bridge(BRIDGE);

            translate([0, 0, M_SWITCH - TACTILE_Z/2- BRIDGE]) bridge(BRIDGE);
            translate([0, 0, M_SWITCH + TACTILE_Z/2]) bridge(BRIDGE);
        }

        // Cut port out.
        translate([-PORT_DX/2, PORT_Y, M_PORT - PORT_CENTER]) 
            cube(size=[PORT_DX, 20, PORT_DZ]);

        // Cut switch out
        translate([-TACTILE_X/2, D_INNER/2-TACTILE_DY, M_SWITCH - TACTILE_Z/2])
            cube(size=[TACTILE_X, 20, TACTILE_Z]);

        TAC_BUF = 4.0;
        translate([-TACTILE_X/2 - TAC_BUF/2, D_INNER/2-TACTILE_DY, M_SWITCH - TACTILE_Z/2 + BRIDGE])
            cube(size=[TACTILE_X + TAC_BUF, 20, TACTILE_Z - BRIDGE*2]);
        
        // Key joint
        translate([0, 0, M_FORE - EPS])
            keyJoint(KEYJOINT + EPS, D_INNER, D_INNER - KEYJOINT_T, true);

        // Bottom access
        hull() {
            translate([0, -4, M_FORE + 14]) rotate([90, 0, 0]) cylinder(h=100, d=16);
            translate([0, -4, M_FORE + 24]) rotate([90, 0, 0]) cylinder(h=100, d=16);
        }
        // side access
        //translate([0, 0, M_FORE + 32]) triCapsule(70, 110, 2, true);
        //translate([0, 0, M_FORE + 22]) triCapsule(70, 110, 2, true);

        // Flat bottom
        translate([-50, -D_INNER/2 -1, M_FORE]) cube(size=[100, 1.5, 100]);
    }

    *color("plum") translate([-PORT_DX/2, PORT_Y, M_PORT - PORT_CENTER]) 
        cube(size=[PORT_DX, PORT_DY, PORT_DZ]);

}

//color("red") translate([-MC_X/2, -MC_Y/2, M_MC]) cube(size=[MC_X, MC_Y, MC_Z]);
//color("green") translate([0, 0, M_BATTERY]) cylinder(h=DZ_BATTERY, d=D_BATTERY);

//color("red") translate([0, 0, M_BOLT]) rotate([-90, 0, 0]) cylinder(h=10, d=2);
//color("red") translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=10, d=12);
//color("red") translate([0, 0, M_PORT]) rotate([-90, 0, 0]) cylinder(h=10, d=9);
