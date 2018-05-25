include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>

$fn = 80;

DZ_PCB = 27;

if (true) {
    pcbHolder(D_AFT, 
            4, 
            DZ_PCB,   // dz
            3,    // dz to pcb
            0,    // dy pcb
            [22, 50, 20],
            [
                [-11.430, 17.280, "buttress" ],     // d=2.2
                [11.430, 17.280, "buttress" ],     // d=2.2
                [-11.430, 2.040, "buttress" ],     // d=2.2
                [11.430, 2.040, "buttress" ],     // d=2.2
            ]
    );
    intersection() {
        cylinder(h=DZ_PCB, d=D_AFT);
        union() {
            translate([-20, 8, DZ_PCB - 3]) cube(size=[40, 20, 10]);
            //translate([7, -20, DZ_PCB - 4]) cube(size=[10, 20, 8]);
            //mirror([1,0,0]) 
            //translate([7, -20, DZ_PCB - 4]) cube(size=[10, 20, 8]);
        }
    }
}

if (true) {
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
                    bridge=(i < N-1)
                );
            }
        }
        translate([0, 0, DZ_PCB]) 
            battery(D_AFT);
    }
    echo("Total len=", (2*N - 1) * H_BUTTRESS + DZ_PCB);
}

