include <dim.scad>
use <../commonUnified.scad>

$fn = 60;

DRAW_AFT = false;
DRAW_FORE = true;

if (DRAW_AFT) {
    translate([0, 0, M_MC]) {
        difference() {
            union() {
                for(i=[0:N_BAFFLES-1]) {
                    translate([0, 0, i*DZ_BUTTRESS*2])  {
                        //oneBaffle(D_INNER, DZ_BUTTRESS, 
                        //        battery=false,
                        //        mc=false,
                        //        bridge=2,
                        //        bottomRail=true
                        //);
                        cylinder(h=DZ_BUTTRESS, d=D_INNER);
                        bridgeAndRail(2, D_INNER, DZ_BUTTRESS, bottomRail=true);

                    }
                }
            }
            // Cut the top
            W = 10.0;
            translate([-W/2, 0, 0]) cube(size=[W, 100, 200]);
            // Cut the bottom
            BW = 12.0;
            translate([-BW/2, -100, 0]) cube(size=[BW, 100, 200]);
        }
    }
}

if (DRAW_FORE)
{
    difference() {        
        translate([0, 0, M_PORT-20])
            powerPortRing(D_INNER, T, M_SWITCH + 5.0 - M_PORT + 20.0, 20.0);
        translate([0, 0, M_SWITCH]) hull() {
            rotate([-90, 0, 0]) cylinder(h=100, d=18);
            translate([0, 0, 20]) rotate([-90, 0, 0]) cylinder(h=100, d=18);
        }
    }
    TAC = 6.2;
    TAC_H = 6.2;    // interesting...
    intersection() {
        cylinder(h=200, d=D_INNER);
        translate([-TAC/2, -D_INNER/2, M_SWITCH - TAC/2]) cube(size=[TAC, D_INNER - TAC_H, TAC]);
    }
}

//color("red") translate([-MC_X/2, -MC_Y/2, M_MC]) cube(size=[MC_X, MC_Y, MC_Z]);
//color("green") translate([0, 0, M_BATTERY]) cylinder(h=DZ_BATTERY, d=D_BATTERY);

//color("red") translate([0, 0, M_BOLT]) rotate([-90, 0, 0]) cylinder(h=10, d=2);
//color("red") translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=10, d=12);
//color("red") translate([0, 0, M_PORT]) rotate([-90, 0, 0]) cylinder(h=10, d=9);
