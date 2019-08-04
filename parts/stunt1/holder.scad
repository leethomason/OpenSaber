include <dim.scad>
use <../shapes.scad>
use <../commonUnified.scad>

Y_POWER = -yAtX(DX_POWER/2, D_AFT/2) + 1;

ROT = ACCESS_PCB ? -25 : 0;
Y_TWEAK = ACCESS_PCB ? -5 : 2;

PCB_DX = 17.5;  // a bit of padding
PCB_DZ = 29.5;

module holder() {    
    difference() {
        intersection() {
            cylinder(h=DZ_PCB, d=D_AFT);
            union() {    
                tube(h=DZ_PCB, do=D_AFT, di=D_AFT - 4);
                
                WPCB = 17.145;  // exact size
                C = -WPCB/2;

                translate([0, Y_TWEAK, 0]) rotate([ROT, 0, 0]) 
                pcbHolder(0, 
                        4, 
                        DZ_PCB,   // dz
                        Z_OFFSET,    // dz to pcb
                        DY_PCB,    // dy pcb
                        [PCB_DX, 50, PCB_DZ],
                        [
                            [C+2.54, 2.54, "buttress" ],     // d=2.2
                            [C+2.54, 26.67, "buttress" ],     // d=2.2
                            [C+14.605, 2.54, "buttress" ],     // d=2.2
                            [C+14.605, 26.67, "buttress" ],     // d=2.2
                        ],
                        makeSection=false
                );

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

        translate([0, Y_TWEAK, 0])  rotate([ROT, 0, 0]) 
            translate([-PCB_DX/2, DY_PCB, Z_OFFSET]) 
                cube(size=[PCB_DX, 50, PCB_DZ*0.95]);

        translate([-DX_POWER/2, Y_POWER, 0]) {
            cube(size=[DX_POWER, DY_POWER, DZ_POWER]);
        }
        translate([0, 0, DZ_PCB]) mirror([0, 0, -1]) 
            keyJoint(JOINT_DZ, D_AFT, D_AFT - JOINT_T, true, 0);    

    }
    *color("green") translate([0, Y_TWEAK, 0])  rotate([ROT, 0, 0]) 
    translate([-27/2, 0, Z_OFFSET]) 
        cube(size=[27, 2, 20]);
}

$fn = 80;
holder();