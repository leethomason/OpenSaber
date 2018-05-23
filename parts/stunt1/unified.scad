include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>

$fn = 80;

C = 19.05 / 2;

if (false) {
    pcbHolder(D_AFT, 
            4, 
            26,   // dz
            3,    // dz to pcb
            0,    // dy pcb
            [22, 50, 20],
            [
                [3.81 - C,   2.54, "buttress"],
                [19.05 - C,  2.54, "buttress"],
                [3.81 - C,  17.78, "buttress"],
                [19.05 - C, 17.78, "buttress"]
            ] );
}

if (true) {
    N = 9;
    D_BATTERY = 19;
    Z_PADDED_BATTERY = 69;

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
        translate([0, D_AFT/2 - D_BATTERY/2, H_BUTTRESS]) 
            cylinder(d=D_BATTERY, h=Z_PADDED_BATTERY);
        HX = 22; // FIXME
        HY = 11; // FIXME
        DY = -22;  // FIXME
        HZ = 74; // FIXME
        translate([-HX/2, D_AFT/2 + DY, H_BUTTRESS])
            cube(size=[HX, HY, HZ]);
    }
}