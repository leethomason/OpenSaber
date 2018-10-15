include <dim.scad>
use <../shapes.scad>
use <util.scad>
use <../commonUnified.scad>

//$fn = 40;
$fn = 90;

DRAW_AFT = false;
DRAW_MID = true;
DRAW_CAP = false;

// part of mid
DRAW_PLATE = true;

N_BAFFLES = nBafflesNeeded(DZ_BUTTRESS);
DZ_BAFFLES = zLenOfBaffles(N_BAFFLES, DZ_BUTTRESS);
T = 4;
M_FRONT_BAFFLES =  M_AFT_BACK + DZ_SPKR_HOLDER + DZ_BAFFLES;
JOINT = 8;
KEY_ANGLE = 0;

module rods(forward)
{
    D_ROD = 3.6;
    D_TUBE = 6;
    D_TUBE_CAP = 4.6;

    translate([7, -7, 0]) cylinder(h=10, d=D_ROD);
    translate([-7,-7, 0]) cylinder(h=10, d=D_ROD);
    translate([0,  10, 0]) cylinder(h=10, d=D_ROD);

    translate([0,  0, 0]) cylinder(h=10, d=D_TUBE_CAP);
    if (forward)
        cylinder(h=2, d=D_TUBE);
    else
        translate([0, 0, 2]) cylinder(h=10, d=D_TUBE);
}

if (DRAW_AFT) {
    translate([0, 0, M_AFT_BACK]) 
        speakerHolder(D_AFT_MID, DZ_SPKR_HOLDER, 1.5, "cls28");

    translate([0, 0, M_AFT_BACK + DZ_SPKR_HOLDER])
        baffleMCBattery(D_AFT_MID, N_BAFFLES, DZ_BUTTRESS);

    translate([0, 0, M_FRONT_BAFFLES])
        cylinderKeyJoint(JOINT, D_AFT_MID, D_AFT_MID - T, 
                         0.2, KEY_ANGLE, true);
}

if (DRAW_MID) {
    JUNCTION = 3;
    TOP_JUNCTION = 1.5;
    DZ_PCB = 0;
    DY_PCB = 7;
    DZ_PCB_OPENING = M_MID_FRONT - M_FRONT_BAFFLES - TOP_JUNCTION - DZ_PCB;
    Z_OFFSET = 1.0;
    echo("DZ_PCB_OPENING", DZ_PCB_OPENING);

    difference() {        
        translate([0, 0, M_FRONT_BAFFLES]) {
            union() {
                pcbHolder(
                    D_AFT_MID, 
                    T,                    
                    M_MID_FRONT - M_FRONT_BAFFLES,
                    DZ_PCB,  // dz PCB
                    DY_PCB,  // dy PCB
                    [23.0, 50, DZ_PCB_OPENING], // fixme
                    [
                        [22.86 / 2 - 19.685, 2.54 + Z_OFFSET, "buttress"],
                        [22.86 / 2 - 3.175,  2.54 + Z_OFFSET, "buttress"],
                        [22.86 / 2 - 19.685, 22.225 + Z_OFFSET, "buttress"],
                        [22.86 / 2 - 3.175,  22.225 + Z_OFFSET, "buttress"]
                    ]
                );
                // keep that battery in place:
                intersection() {
                    cylinder(h=10, d=D_AFT_MID);
                    union() {
                        translate([6, -20, 0]) cube(size=[20, 24, 2]);
                        mirror([1,0,0])
                        translate([6, -20, 0]) cube(size=[20, 24, 2]);
                    }
                }
            }
        }
        
        translate([0, 0, M_FRONT_BAFFLES])
            cylinderKeyJoint(JOINT, D_AFT_MID, D_AFT_MID - T, 
                             0, KEY_ANGLE, true);

        // Side holes
        SIDE_TRIM = -0.8;
        translate([0, 0, DZ_MID/2 + M_MID_BACK + SIDE_TRIM]) rotate([0, 90, 0]) cylinder(h=100, d=4);
        translate([0, 0, DZ_MID/2 + M_MID_BACK + SIDE_TRIM]) rotate([0, -90, 0]) cylinder(h=100, d=4);
        // Bottom access
        translate([0, 0, M_MID_BACK + DZ_PCB + 7.62 + Z_OFFSET]) rotate([90, 0, 0]) cylinder(h=100, d=12);

        // Hint of flatness
        translate([-20, -D_AFT_MID/2 - 1, M_FRONT_BAFFLES]) cube(size=[40, 1 + 0.5, M_MID_FRONT - M_FRONT_BAFFLES]);
    }

    if (DRAW_PLATE) {

        translate([0, 0, M_MID_FRONT - JUNCTION]) {
            difference() {
                union() {
                    translate([0, 0, JUNCTION - TOP_JUNCTION])
                        cylinder(h=TOP_JUNCTION, d=D_AFT_MID);
                    intersection() {
                        cylinder(h=JUNCTION, d=D_AFT_MID);
                        translate([-20, -20, 0]) 
                            cube(size=[40, 20 + DY_PCB - 1, JUNCTION]);
                    }
                }
                rods(false);
                // Hint of flatness
                translate([-20, -D_AFT_MID/2 - 1, 0]) cube(size=[40, 1 + 0.5, JUNCTION]);
            }
        }
    }
}

if (DRAW_CAP) {
    translate([0, 0, M_FORE_BACK + 20]) {
        difference() {
            cylinder(h=DZ_BUTTRESS, d=D_FORE);
            rods(true);
        }
    }
}


*inner();
*translate([0, 0, M_AFT_BACK + DZ_SPKR_HOLDER])
    battery(D_AFT_MID);

*translate([-PLATE_DX/2, 0, M_MID_CENTER - PLATE_MOUNT/2])
    color("orange")
        cube(size=[PLATE_DX, 20, PLATE_MOUNT]);

*translate([0, 0, M_MID_BACK]) cylinder(h=DZ_MID, d=D_AFT_MID);
