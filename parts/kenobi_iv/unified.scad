include <dim.scad>
use <../shapes.scad>
use <util.scad>
use <../commonUnified.scad>

$fn = 40;

DRAW_AFT = true;

N_BAFFLES = nBafflesNeeded(DZ_BUTTRESS);

if (DRAW_AFT) {
    difference() {
        union() {
            translate([0, 0, M_AFT_BACK]) 
                speakerHolder(D_AFT_MID, DZ_SPKR_HOLDER, 1, "cls28");

            translate([0, 0, M_AFT_BACK + DZ_SPKR_HOLDER])
                baffleMCBattery(D_AFT_MID, N_BAFFLES, DZ_BUTTRESS);

            //   oneBaffle(D_AFT, 6, battery=false, mc=true, useRods=false, bridge=true, cutoutHigh=false);
            /*
            translate([0, 0, M_POMMEL_FRONT + Z_AFT_RING]) {
                oneBaffle(D_AFT, Z_SPACER, false, true, false, true, cutoutHigh=false);
                translate([0, 0, Z_SPACER])
                    baffleMCBattery(D_AFT, N_BAFFLES, H_BUTTRESS, 
                                    D_AFT, H_BUTTRESS, EXTRA_BAFFLE, mcWide=19.5);
            }

            translate([0, 0, M_POMMEL_FRONT]) {
                speakerHolder(D_AFT_RING, Z_AFT_RING, 1, "cls28");
            }

            translate([0, 0, M_BAFFLE_FRONT]) {
                cylinderKeyJoint(JUNCTION, D_AFT, D_AFT - T, 0.5, JOINT_ANGLE);
            }*/
        }
        //translate([0, 0, M_AFT_BACK + 1])
        //    speakerCLS28(true);
    }
}

*inner();
translate([0, 0, M_AFT_BACK + DZ_SPKR_HOLDER])
    battery(D_AFT_MID);

translate([-PLATE_DX/2, 0, M_MID_CENTER - PLATE_MOUNT/2])
    color("orange")
        cube(size=[PLATE_DX, 20, PLATE_MOUNT]);

