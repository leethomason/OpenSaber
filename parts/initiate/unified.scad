include <dim.scad>
use <../commonUnified.scad>

EPS = 0.01;
$fn = 60;

N_BAFFLES = 10;
DZ_BUTTRESS = DZ_AFT / (N_BAFFLES*2 + 1);
T = 4.0;

PORT_DX = 14.50;
PORT_DY =  9.00;
PORT_DZ = 10.70;
D_BATTERY  = 18.50;

difference() {
    union() {
        translate([0, 0, M_AFT]) {
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
    translate([-PORT_DX/2, D_INNER/2 - D_BATTERY - PORT_DY, 0]) cube(size=[PORT_DX, PORT_DY, PORT_DZ]);
}

color("green") translate([0, 0, M_AFT]) battery(D_INNER, "18500");