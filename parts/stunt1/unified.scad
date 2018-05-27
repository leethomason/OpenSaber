include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>

$fn = 80;

DZ_PCB = 27;

DX_POWER = 11;
DY_POWER = 9;
DZ_POWER = 14;

Y_POWER = -yAtX(DX_POWER/2, D_AFT/2) + 1;

if (true) {
    difference() {
        pcbHolder(D_AFT, 
                4, 
                DZ_PCB,   // dz
                3,    // dz to pcb
                0,    // dy pcb
                [27, 50, 20],
                [
                    [-11.430, 17.280, "buttress" ],     // d=2.2
                    [11.430, 17.280, "buttress" ],     // d=2.2
                    [-11.430, 2.040, "buttress" ],     // d=2.2
                    [11.430, 2.040, "buttress" ],     // d=2.2
                ]
        );

        translate([-DX_POWER/2, Y_POWER, 0]) {
            cube(size=[DX_POWER, DY_POWER, DZ_POWER]);
        }
    }
    intersection() {
        cylinder(h=DZ_PCB, d=D_AFT);
        union() {
            translate([-20, 8, DZ_PCB - 3]) cube(size=[40, 20, 10]);
            translate([DX_POWER/2, Y_POWER, 0]) 
                cube(size=[4, DY_POWER, DZ_POWER]);
            mirror([1,0,0]) translate([DX_POWER/2, Y_POWER, 0]) 
                cube(size=[4, DY_POWER, DZ_POWER]);
        }
    }
}

if (false) {
    EXTRA_BAFFLE = 2;
    N_BAFFLES = nBafflesNeeded(H_BUTTRESS);
    N = N_BAFFLES + 1;

    difference() {
        for(i=[0:N-1]) {
            translate([0, 0, i*H_BUTTRESS*2 + DZ_PCB]) {
                oneBaffle(
                    D_AFT,
                    H_BUTTRESS,
                    battery=false,
                    mc=false,
                    bridge=(i < N-1),
                    scallop=true
                );
            }
        }
        translate([0, 0, DZ_PCB]) 
            battery(D_AFT);
    }
    DZAFT = (2*N - 1) * H_BUTTRESS;
    translate([0, 0, DZ_PCB]) intersection() {
        cylinder(h=DZAFT, d=D_AFT);
        translate([-20, -16, 0]) cube(size=[40, 3, DZAFT]);
    }
    
    echo("Total len=", (2*N - 1) * H_BUTTRESS + DZ_PCB);
}

