include <dim.scad>
use <../shapes.scad>
use <util.scad>
use <../commonUnified.scad>

//$fn = 40;
$fn = 90;

DRAW_AFT = true;
DRAW_MID = true;
DRAW_BAFFLE = false;
DRAW_CAP = false;

N_BAFFLES = nBafflesNeeded(DZ_BUTTRESS);
DZ_BAFFLES = zLenOfBaffles(N_BAFFLES, DZ_BUTTRESS);
T = 4;
M_FRONT_BAFFLES =  M_AFT_BACK + DZ_SPKR_HOLDER + DZ_BAFFLES;
JOINT = 8;
KEY_ANGLE = 0;

Y_TUBE = -9;

module rods(cap)
{
    D_TUBE_CAP = 4.6;

    translate([0, 0, 0]) cylinder(h=10, d=D_ROD);

    if (cap == "aft")
        translate([0, Y_TUBE, 1.5]) cylinder(h=10, d=D_TUBE);
    else if (cap == "fore")
        translate([0, Y_TUBE, 0]) cylinder(h=1.5, d=D_TUBE);
    else
        translate([0, Y_TUBE, 0]) cylinder(h=20, d=D_TUBE);

    translate([0, Y_TUBE, 0]) cylinder(h=20, d=D_TUBE_CAP);
}

module cap()
{
    difference() {
        union() {
            wingedCylinder();
            translate([0, 0, CAP_DZ]) notch(-0.3);
        }

        notch();

        // Space for brass
        translate([0, 0, CAP_DZ - CAP_BRASS])
            cylinder(h=10, d=CAP_D_INNER + 8);

        // Rods.
        rods();
    }
}

if (DRAW_AFT) {
    translate([0, 0, M_AFT_BACK]) 
        speakerHolder(D_AFT_MID, DZ_SPKR_HOLDER, 1.5, "cls28");

    translate([0, 0, M_AFT_BACK + DZ_SPKR_HOLDER])
        baffleMCBattery(D_AFT_MID, N_BAFFLES, DZ_BUTTRESS, bridgeStyle=2);

    translate([0, 0, M_FRONT_BAFFLES])
        keyJoint(JOINT, D_AFT_MID, D_AFT_MID - T, false, KEY_ANGLE);
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
            keyJoint(JOINT, D_AFT_MID, D_AFT_MID - T, true, KEY_ANGLE);

        // Side holes
        SIDE_TRIM = -0.8;
        translate([0, 0, DZ_MID/2 + M_MID_BACK + SIDE_TRIM]) rotate([0, 90, 0]) cylinder(h=100, d=4);
        translate([0, 0, DZ_MID/2 + M_MID_BACK + SIDE_TRIM]) rotate([0, -90, 0]) cylinder(h=100, d=4);
        // Bottom access
        translate([0, 0, M_MID_BACK + DZ_PCB + 7.62 + Z_OFFSET]) rotate([90, 0, 0]) cylinder(h=100, d=12);

        // Hint of flatness
        translate([-20, -D_AFT_MID/2 - 1, M_FRONT_BAFFLES]) cube(size=[40, 1 + 0.5, M_MID_FRONT - M_FRONT_BAFFLES]);
    }

    // Draw the end plate.
    if (true) {
        translate([0, 0, M_MID_FRONT - JUNCTION]) {
            difference() {
                union() {
                    translate([0, 0, JUNCTION - TOP_JUNCTION])
                        cylinder(h=TOP_JUNCTION, d=D_AFT_MID);

                    translate([0, 0, JUNCTION - TOP_JUNCTION])
                        cap();

                    intersection() {
                        cylinder(h=JUNCTION, d=D_AFT_MID);
                        translate([-20, -20, 0]) 
                            cube(size=[40, 20 + DY_PCB - 1, JUNCTION]);
                    }
                }
                rods("aft");
                // Hint of flatness
                translate([-20, -D_AFT_MID/2 - 1, 0]) cube(size=[40, 1 + 0.5, JUNCTION]);
            }
        }
    }
}

module wingedCylinder()
{
    cylinder(h=CAP_DZ, d=CAP_D_INNER);
    translate([0, Y_TUBE, 0])
        tube(h=CAP_DZ, do=D_TUBE + 1.5, di=D_TUBE);

    difference() 
    {
        CARVE_ANGLE = 7;    // corret only on the outer. was 8 - tight
        THETA = CAP_THETA + CARVE_ANGLE;

        cylinder(h=CAP_DZ, d=D_FORE);
        translate([0, 0, -1]) polygonXY(h=10, points=[
            [0, 0],
            [cos(180 - THETA) * 100, sin(180 - THETA) * 100],
            [cos(THETA) * 100, sin(THETA) * 100],
        ]);
        translate([0, 0, -1]) polygonXY(h=10, points=[
            [0, 0],
            [cos(-THETA) * 100, sin(-THETA) * 100],
            [cos(180 + THETA) * 100, sin(180 + THETA) * 100],
        ]);
    }
}

module notch(trim=0)
{
    H = 1.5;
    intersection() {
        translate([-20, -2 - trim/2, 0]) cube(size=[40, 4 + trim, H]);
        tube(h=10, do=D_FORE, di=CAP_D_INNER);
    }
}



if (DRAW_BAFFLE) {
    translate([0, 0, M_FORE_BACK + 20]) {
        cap();
    }

    // Draw the brass part:
    *translate([0, 0, M_FORE_BACK + 19])
    {
        difference() {
            color("gold") {
                cylinder(h=1, d=CAP_D_INNER);
                difference() {
                    cylinder(h=1, d=D_FORE);
                    wingedCylinder();
                }
            }
            rods();
        }
    }
}

if (DRAW_BAFFLE) {
    translate([0, 0, M_FORE_BACK + 20]) {
        cap();
    }

    // Draw the brass part:
    *translate([0, 0, M_FORE_BACK + 19])
    {
        difference() {
            color("gold") {
                cylinder(h=1, d=CAP_D_INNER);
                difference() {
                    cylinder(h=1, d=D_FORE);
                    wingedCylinder();
                }
            }
            rods();
        }
    }
}


if (DRAW_CAP) {
    translate([0, 0, M_FORE_BACK + 20 + CAP_DZ]) {
        difference() {
            cylinder(h=CAP_DZ, d=D_FORE);
            notch();
            rods("fore");        
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
