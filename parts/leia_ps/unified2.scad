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

PCB_W = 16;
PCB_Y = D_INNER/2 - 8.5;
M_JOINT = M_FORE + 6.0;
DZ_JOINT = 5.0;
TOP_W = 8.0;

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
            translate([-TOP_W/2, 0, 0]) //DZ_BUTTRESS*3]) 
                cube(size=[TOP_W, 100, 200]);

            // Cut the bottom
            BW = 12.0;
            translate([-BW/2, -100, DZ_BUTTRESS*3]) cube(size=[BW, 100, 200]);
        }
        translate([-MC_X/2, -2, M_MC]) 
            cube(size=[MC_X, MC_Y, MC_Z + 10]);
        translate([0, -0.5 * (D_INNER - D_BATTERY) / 2.0, M_BATTERY]) 
            cylinder(h=DZ_BATTERY, d=D_BATTERY);
    }
    difference() {
        translate([0, 0, M_FORE - EPS])
            cylinder(h=M_JOINT + DZ_JOINT - M_FORE, d=D_INNER);
        translate([-TOP_W/2, -50, 0])
            cube(size=[TOP_W, 100, 200]);
        translate([0, -50, M_FORE])
            cube(size=[PCB_W/2, 100, M_JOINT - M_FORE]);
        mirror([-1, 0, 0]) translate([0, -50, M_FORE])
            cube(size=[PCB_W/2, 100, M_JOINT - M_FORE]);
    }
}

//color("red") translate([-MC_X/2, -MC_Y/2, M_MC]) cube(size=[MC_X, MC_Y, MC_Z]);
//color("green") translate([0, 0, M_BATTERY]) cylinder(h=DZ_BATTERY, d=D_BATTERY);

color("red") translate([0, 0, M_BOLT]) rotate([-90, 0, 0]) cylinder(h=10, d=3);
color("red") translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=10, d=12);
color("red") translate([0, 0, M_PORT]) rotate([-90, 0, 0]) cylinder(h=10, d=9);
color("red") translate([0, 0, M_BOLT_1]) rotate([-90, 0, 0]) cylinder(h=10, d=3);

difference() {
    color("green") translate([-PCB_W/2, PCB_Y, M_FORE])
        cube(size=[PCB_W, 1.6, M_BOLT_1 - M_BOLT + 10.0]);
    translate([TOP_W/2, PCB_Y-EPS, M_JOINT])
        cube(size=[20, 20, DZ_JOINT]);
    mirror([-1, 0, 0]) translate([TOP_W/2, PCB_Y-EPS, M_JOINT])
        cube(size=[20, 20, DZ_JOINT]);
}

// 4mm drop
// 5.5mm barrel on power - 1.6 = 4.0 // if nut fits