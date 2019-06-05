include <dim.scad>
use <../shapes.scad>
use <../commonUnified.scad>

// Duped in unified.
Y_POWER = -yAtX(DX_POWER/2, D_AFT/2) + 1;

module holder() {    
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
            translate([-28/2, DY_PCB, Z_OFFSET]) 
                cube(size=[28, 50, 22]);

        translate([-DX_POWER/2, Y_POWER, 0]) {
            cube(size=[DX_POWER, DY_POWER, DZ_POWER]);
        }
        // flat bottom
        translate([-20, -20.5, 0]) cube(size=[40, 5, 100]);

        translate([0, 0, DZ_PCB]) mirror([0, 0, -1]) keyJoint(JOINT_DZ, D_AFT, D_AFT - JOINT_T, 0, 0, true);    

    }
    *color("green") translate([0, Y_TWEAK, 0])  rotate([ROT, 0, 0]) 
    translate([-27/2, 0, Z_OFFSET]) 
        cube(size=[27, 2, 20]);
}