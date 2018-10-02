include <dim.scad>
use <../shapes.scad>
use <util.scad>
use <../commonUnified.scad>

//$fn = 40;
$fn = 90;

DRAW_AFT = false;
DRAW_MID = true;

N_BAFFLES = nBafflesNeeded(DZ_BUTTRESS);
DZ_BAFFLES = zLenOfBaffles(N_BAFFLES, DZ_BUTTRESS);
T = 4;
M_FRONT_BAFFLES =  M_AFT_BACK + DZ_SPKR_HOLDER + DZ_BAFFLES;
JOINT = 5;
KEY_ANGLE = -20;

if (DRAW_AFT) {
    translate([0, 0, M_AFT_BACK]) 
        speakerHolder(D_AFT_MID, DZ_SPKR_HOLDER, 1, "cls28");

    translate([0, 0, M_AFT_BACK + DZ_SPKR_HOLDER])
        baffleMCBattery(D_AFT_MID, N_BAFFLES, DZ_BUTTRESS);

    translate([0, 0, M_FRONT_BAFFLES])
        cylinderKeyJoint(JOINT, D_AFT_MID, D_AFT_MID - T, 0.5, KEY_ANGLE);
}

if (DRAW_MID) {
    DZ_PCB = 1;
    difference() {
        translate([0, 0, M_FRONT_BAFFLES]) {
            pcbHolder(
                D_AFT_MID, 
                T,                    
                M_MID_FRONT - M_FRONT_BAFFLES,  // arbitrary; there is more space
                DZ_PCB,  // dz PCB
                5,  // dy PCB
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
}


*inner();
*translate([0, 0, M_AFT_BACK + DZ_SPKR_HOLDER])
    battery(D_AFT_MID);

*translate([-PLATE_DX/2, 0, M_MID_CENTER - PLATE_MOUNT/2])
    color("orange")
        cube(size=[PLATE_DX, 20, PLATE_MOUNT]);

*translate([0, 0, M_MID_BACK]) cylinder(h=DZ_MID, d=D_AFT_MID);
