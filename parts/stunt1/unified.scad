include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>

$fn = 80;

if (true) {
    pcbHolder(D_AFT, 
            4, 
            26,   // dz
            3,    // dz to pcb
            0,    // dy pcb
            [22, 50, 20],
            [
                [7.620, 17.280, "buttress" ],     // d=2.2
                [-7.620, 17.280, "buttress" ],     // d=2.2
                [7.620, 2.040, "buttress" ],     // d=2.2
                [-7.620, 2.040, "buttress" ],     // d=2.2
            ]
    );
}

if (false) {
    EXTRA_BAFFLE = 2;
    N_BAFFLES = nBafflesNeeded(H_BUTTRESS);
    N = N_BAFFLES;

    difference() {
        for(i=[0:N-1]) {
            translate([0, 0, i*H_BUTTRESS*2]) {
                oneBaffle(
                    D_AFT,
                    H_BUTTRESS,
                    battery=false,
                    mc=false,
                    bridge=(i < N-1)
                );
            }
        }
        translate([0, 0, H_BUTTRESS]) 
            battery(D_AFT);
    }
}