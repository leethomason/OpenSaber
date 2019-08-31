use <../shapes.scad>
use <../commonUnified.scad>

DX_POWER = 11;
DY_POWER = 9;
DZ_POWER = 14;
DY_PCB = 0;
Z_OFFSET = 3;

// Version v1b
W_PCB = 17.78;  // exact size
C_PCB = -W_PCB/2;
PCB_DX = 17.9;  // a bit of padding
PCB_DZ = 29.5;

T_BATT_STOP = 3;
JOINT_T = 3;
JOINT_DZ = 8;

ROT = -10;
Y_TWEAK = -1;

module holder(diameter, dzPCB) {    
    Y_POWER = -yAtX(DX_POWER/2, diameter/2) + 1;

    difference() {
        intersection() {
            cylinder(h=dzPCB, d=diameter);
            union() {    
                difference() {
                    tube(h=dzPCB-T_BATT_STOP, do=diameter, di=diameter - 4);
                    translate([-PCB_DX/2, DY_PCB, 0]) 
                        cube(size=[PCB_DX, 50, dzPCB-T_BATT_STOP]);
                }
                
                translate([0, Y_TWEAK, 0]) rotate([ROT, 0, 0]) 
                pcbHolder(0, 
                        4, 
                        dzPCB,   // dz
                        Z_OFFSET,    // dz to pcb
                        DY_PCB,    // dy pcb
                        [PCB_DX, 50, PCB_DZ],
                        [
                            [C_PCB + 2.54, 2.54, "buttress" ],     // d=2.2
                            [C_PCB + 2.54, 26.67, "buttress" ],     // d=2.2
                            [C_PCB + 15.24, 2.54, "buttress" ],     // d=2.2
                            [C_PCB + 15.24, 26.67, "buttress" ],     // d=2.2
                        ],
                        makeSection=false
                );

                // Battery stop.
                translate([0, 0, dzPCB-T_BATT_STOP]) 
                    oneBaffle(diameter, T_BATT_STOP, battery=false, mc=false, bridge=0, noBottom=false, bottomRail=false, conduit=true);

                translate([DX_POWER/2, Y_POWER, 0]) 
                    cube(size=[4, DY_POWER, DZ_POWER]);
                mirror([1,0,0]) translate([DX_POWER/2, Y_POWER, 0]) 
                    cube(size=[4, DY_POWER, DZ_POWER]);
                
                translate([-10, Y_POWER, DZ_POWER])
                    cube(size=[20, 1, 2]);
            }
        }

        translate([-DX_POWER/2, Y_POWER, 0]) {
            cube(size=[DX_POWER, DY_POWER, DZ_POWER]);
            translate([0, 1.5, 0])
                cube(size=[DX_POWER, DY_POWER-1.5, 100]);
        }
        translate([0, 0, dzPCB]) mirror([0, 0, -1]) 
            keyJoint(JOINT_DZ, diameter, diameter - JOINT_T, true, 0);    

    }
    *color("green") translate([0, Y_TWEAK, 0])  rotate([ROT, 0, 0]) 
    *translate([-27/2, 0, Z_OFFSET]) 
        cube(size=[27, 2, 20]);
}

$fn = 80;
holder();