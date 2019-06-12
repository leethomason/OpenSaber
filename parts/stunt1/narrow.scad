use <../commonUnified.scad>
use <../shapes.scad>
use <holder.scad>

DZ_AFT = 188.0;
D_AFT = 22.2;
DZ_PCB_SECTION = 32;
DZ_CUTOUT = 26;
DX_CUTOUT = 18;
H_BUTTRESS = 4;
Z_BATTERY = 69.0;
JOINT_DZ = 8;
JOINT_T = 3;

DRAW_HOLDER = false;
DRAW_BODY   = true;

$fn = 80;
EPS = .01;

module innerSpace()
{
    cylinder(h=500, d=D_AFT);
}

if (DRAW_BODY) {

    N_BAFFLES = 10;

    translate([0, 0, DZ_PCB_SECTION]) {
        difference() {
            union() {
                for(i=[0:N_BAFFLES - 1]) {
                    z = i*H_BUTTRESS*2;
                    translate([0, 0, z]) {
                        h = i < N_BAFFLES - 1 ? H_BUTTRESS : H_BUTTRESS * 1.5;
                        cylinder(h=h, d=D_AFT);
                    }
                }
                // Rails
                intersection() {
                    innerSpace();
                    //union() {
                    //    translate([10, -3, 0]) cylinder(h=Z_BATTERY+4, d=5);
                    //    translate([-10, -3, 0]) cylinder(h=Z_BATTERY+4, d=5);
                    //}
                    translate([-50, -50, 0]) cube(size=[100, 45, Z_BATTERY + 4]);
                }
            }
            // Cut space for the battery
            battery(D_AFT);
            translate([0, 0, 4]) battery(D_AFT);
            // Top cut:
            W = 10;        
            translate([-W/2, 0, 0]) cube(size=[W, 20, Z_BATTERY]);
            // Buttom cut - entire length for wiring
            W = 5;
            translate([-W/2, -20, 0]) cube(size=[W, 20, 200]);
            // Trim baffles
            W = 10;
            translate([-W/2, 0, 0]) cube(size=[W, 50, Z_BATTERY]);
        }
        mirror([0, 0, -1])
            keyJoint(JOINT_DZ, D_AFT, D_AFT - JOINT_T, 0.1, 0, true);    
    }
}

if (DRAW_HOLDER) {
    DI = D_AFT - 4;
    D_RING = 27.5;
    DZ_RING = 12.0 + 3.2 - 9.8;
    T = 2;
    DX_POWER = 11;
    DY_POWER = 9;
    DZ_POWER = 14;

    difference() {
        union() {
            tube(h=DZ_PCB_SECTION, do=D_AFT, di=DI);

            translate([0, 0, -DZ_RING]) {
                tube(h=DZ_RING, do=D_RING, di=DI);
            }
            translate([-DX_POWER/2 - T, -9, -DZ_RING - EPS]) {
                cube(size=[DX_POWER + 2*T, 2, DZ_POWER + T]);
            }
        }

        translate([-DX_CUTOUT/2, 0, (DZ_PCB_SECTION-DZ_CUTOUT)/2])
            cube(size=[DX_CUTOUT, 100, DZ_CUTOUT]);
        W = 5;
        translate([-W/2, -20, 20]) cube(size=[W, 20, 200]);

        translate([0, 0, DZ_PCB_SECTION])
            mirror([0, 0, -1])
                keyJoint(JOINT_DZ, D_AFT, D_AFT - JOINT_T, 0, 0, true);    

        translate([-DX_POWER/2, -9, -DZ_RING - EPS]) {
            cube(size=[DX_POWER, DY_POWER, DZ_POWER]);
        }
    }
}

*color("yellow") translate([0, 0, DZ_PCB]) battery(D_FORE);
*color("red") translate([0, 0, DZ_PCB - EPS]) AAHolder();