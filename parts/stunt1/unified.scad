include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>

// TODO: add stops for power
// TODO: add key to connect 2 parts
// TODO: a little more space for the 'holder' battery stop

$fn = 80;

DZ_PCB = 27;
Z_OFFSET = 3;

DX_POWER = 11;
DY_POWER = 9;
DZ_POWER = 14;

DY_PCB = 0;
Y_POWER = -yAtX(DX_POWER/2, D_AFT/2) + 1;

ROT = -25;
Y_TWEAK = -5;

module key(out)
{
    KL = 10;
    intersection() 
    {
        tube(h=KL, do=D_AFT, di=out ? D_AFT-4 : 0);
        translate([-4, -20, 0])
            cube(size=[8, 20, KL]);
    }
}

if (false) {
    difference() {
        intersection() {
            cylinder(h=DZ_PCB, d=D_AFT);
            union() {    
                tube(h=DZ_PCB, do=D_AFT, di=D_AFT - 4);
                
                translate([0, Y_TWEAK, 0]) rotate([ROT, 0, 0]) 
                pcbHolder(0, 
                        4, 
                        DZ_PCB,   // dz
                        Z_OFFSET,    // dz to pcb
                        DY_PCB,    // dy pcb
                        [27, 50, 20],
                        [
                            [-11.430, 17.280, "buttress" ],     // d=2.2
                            [11.430, 17.280, "buttress" ],     // d=2.2
                            [-11.430, 2.040, "buttress" ],     // d=2.2
                            [11.430, 2.040, "buttress" ],     // d=2.2
                        ],
                        makeSection=false
                );
            }
        }

        translate([0, Y_TWEAK, 0])  rotate([ROT, 0, 0]) 
            translate([-28/2, DY_PCB, Z_OFFSET]) 
                cube(size=[28, 50, 22]);

        translate([-DX_POWER/2, Y_POWER, 0]) {
            cube(size=[DX_POWER, DY_POWER, DZ_POWER]);
        }
    }
    *color("green") translate([0, Y_TWEAK, 0])  rotate([ROT, 0, 0]) 
    translate([-27/2, 0, Z_OFFSET]) 
        cube(size=[27, 2, 20]);
    
    intersection() {
        cylinder(h=DZ_PCB, d=D_AFT);
        union() {
            // Battery stop.
            translate([-20, -20, DZ_PCB - 3]) 
                cube(size=[12, 40, 10]);
            mirror([1,0,0]) translate([-20, -20, DZ_PCB - 3]) 
                cube(size=[12, 40, 10]);
            
            
            translate([DX_POWER/2, Y_POWER, 0]) 
                cube(size=[4, DY_POWER, DZ_POWER]);
            mirror([1,0,0]) translate([DX_POWER/2, Y_POWER, 0]) 
                cube(size=[4, DY_POWER, DZ_POWER]);
            
            translate([-10, Y_POWER, DZ_POWER])
                cube(size=[20, 1, 2]);
        }
    }
    
    translate([0, 0, DZ_PCB]) key(true);
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
                    mc=true,
                    bridge=(i < N-1),
                    scallop=true,
                    cutout=false
                );
            }
        }
        translate([0, 0, DZ_PCB]) 
            battery(D_AFT);
        W = 10;
        translate([-W/2, -5, 0])
            cube(size=[W, 16, 120]);
        
        // flat bottom
        translate([-20, -20, 0])
            cube(size=[40, 5, 100]);
        
        translate([0, 0, DZ_PCB]) key(false);
    }
    DZAFT = (2*N - 1) * H_BUTTRESS;
    
*    translate([0, 0, DZ_PCB]) {
        difference() {
            intersection() {
                cylinder(h=DZAFT, d=D_AFT);
                translate([-20, -16, 0]) cube(size=[40, 5, DZAFT]);
            }
            key(false);
        }
    }
    
    echo("Total len=", (2*N - 1) * H_BUTTRESS + DZ_PCB);
}

