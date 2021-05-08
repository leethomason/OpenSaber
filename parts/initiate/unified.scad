include <dim.scad>
use <../commonUnified.scad>

EPS = 0.01;
$fn = 60;

N_BAFFLES = 10;
DZ_BUTTRESS = DZ_AFT / (N_BAFFLES*2 + 1);
T = 4.0;

PORT_DX = 10.8;
PORT_DY =  8.8;
PORT_DZ = 13.4; // excluding contacts
D_BATTERY  = 18.50;
DZ_BATTERY = 55.0;

PCB_DX = 17.8 + 0.6;
PCB_DZ = 29.5;
M_PCB = 27.8;

module pcbHole()
{
    rotate([-90, 0, 0]) cylinder(h=20.0, d=2.2);
}

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
    // PCB
    translate([-PCB_DX/2, -D_INNER/2, M_PCB])
        cube(size=[PCB_DX, 8.0, PCB_DZ]);

    HOLE_DX = (0.7 - 0.1) * 0.5 * 25.4;
    HOLE_DZ0 = 0.1 * 25.4;
    HOLE_DZ1 = 1.05 * 25.4;

    translate([-HOLE_DX, -D_INNER/2, M_PCB + HOLE_DZ0]) pcbHole();
    translate([HOLE_DX, -D_INNER/2, M_PCB + HOLE_DZ0]) pcbHole();
    translate([-HOLE_DX, -D_INNER/2, M_PCB + HOLE_DZ1]) pcbHole();

    // Back port
    PORT_EXTRA = 10.0;
    translate([-PORT_DX/2, D_INNER/2 - D_BATTERY - PORT_DY - PORT_EXTRA, M_AFT]) 
        cube(size=[PORT_DX, PORT_DY + PORT_EXTRA, PORT_DZ]);

    // Wire Access
    W = 8.0;
    translate([-W/2, -100.0, 0]) cube(size=[W, 100, 200]);
    W2 = 14.0;
    translate([-W2/2, 0, 0]) cube(size=[W2, 100, DZ_AFT - DZ_BUTTRESS*1]);

    translate([0, 0, M_AFT]) battery(D_INNER, "18500");
}

*color("green") translate([0, 0, M_AFT]) battery(D_INNER, "18500");