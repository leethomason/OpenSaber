include <dim.scad>
use <../shapes.scad>
use <util.scad>
use <../commonUnified.scad>

//$fn = 40;
$fn = 90;

DRAW_AFT = true;
DRAW_MID = true;
DRAW_CAP = true;


// part of mid
DRAW_PLATE = true;

N_BAFFLES = nBafflesNeeded(DZ_BUTTRESS);
DZ_BAFFLES = zLenOfBaffles(N_BAFFLES, DZ_BUTTRESS);
T = 4;
M_FRONT_BAFFLES =  M_AFT_BACK + DZ_SPKR_HOLDER + DZ_BAFFLES;
JOINT = 5;
KEY_ANGLE = -20;

module rods(forward)
{
    translate([7, -7, 0]) cylinder(h=10, d=3);  // fixme rod
    translate([-7,-7, 0]) cylinder(h=10, d=3);  // fixme rod
    translate([0,  9, 0]) cylinder(h=10, d=3);  // fixme rod

    translate([0,  0, 0]) cylinder(h=10, d=4);  // fixme tube
    if (forward)
        cylinder(h=2, d=5);
    else
        translate([0, 0, 2]) cylinder(h=10, d=5);
}

if (DRAW_AFT) {
    translate([0, 0, M_AFT_BACK]) 
        speakerHolder(D_AFT_MID, DZ_SPKR_HOLDER, 1, "cls28");

    translate([0, 0, M_AFT_BACK + DZ_SPKR_HOLDER])
        baffleMCBattery(D_AFT_MID, N_BAFFLES, DZ_BUTTRESS);

    translate([0, 0, M_FRONT_BAFFLES])
        cylinderKeyJoint(JOINT, D_AFT_MID, D_AFT_MID - T, 0.5, KEY_ANGLE);
}

if (DRAW_MID) {
    DZ_PCB = 0;
    DY_PCB = 5;

    difference() {
        translate([0, 0, M_FRONT_BAFFLES]) {
            pcbHolder(
                D_AFT_MID, 
                T,                    
                M_MID_FRONT - M_FRONT_BAFFLES,
                DZ_PCB,  // dz PCB
                DY_PCB,  // dy PCB
                [23.0, 50, 23.0],        // fixme: guess
                [
                    [22.86 / 2 - 19.685, 4, "buttress"],
                    [22.86 / 2 - 3.175,  4, "buttress"],
                    [22.86 / 2 - 19.685, 19.685, "buttress"],
                    [22.86 / 2 - 3.175,  19.685, "buttress"]
                ]
            );
        }
        translate([0, 0, M_FRONT_BAFFLES])
            cylinderKeyJoint(JOINT, D_AFT_MID, D_AFT_MID - T, 0.5, KEY_ANGLE);

        translate([0, 0, DZ_MID/2 + M_MID_BACK]) rotate([0, 90, 0]) cylinder(h=100, d=4);    // fixme - tube outer D
        translate([0, 0, DZ_MID/2 + M_MID_BACK]) rotate([0, -90, 0]) cylinder(h=100, d=4);    // fixme - tube outer D
        translate([0, 0, M_MID_BACK + DZ_PCB + 8.89]) rotate([90, 0, 0]) cylinder(h=100, d=12);
    }

    if (DRAW_PLATE) {
        JUNCTION = 3;

        translate([0, 0, M_MID_FRONT - JUNCTION]) {
            difference() {
                union() {
                    translate([0, 0, JUNCTION - 1])
                        cylinder(h=1, d=D_AFT_MID);
                    intersection() {
                        cylinder(h=JUNCTION, d=D_AFT_MID);
                        translate([-20, -20, 0]) 
                            cube(size=[40, 20 + DY_PCB - 1, JUNCTION]);
                    }
                }
                rods(false);
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
