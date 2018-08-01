use <../shapes.scad>
include <dim.scad>
use <../commonUnified.scad>

$fn = 90;
EPS = 0.01;
EPS2 = EPS * 2;

DRAW_AFT = false;
DRAW_FRONT = true;

T = 4;
JUNCTION = 6;
EXTRA_BAFFLE = 2;
Z_AFT_RING = 5.5;       // also speaker holder
Z_SPACER = 2.5;
N_BAFFLES = nBafflesNeeded(H_BUTTRESS);
M_BAFFLE_FRONT = zLenOfBaffles(N_BAFFLES, H_BUTTRESS) 
    + M_POMMEL_FRONT + EXTRA_BAFFLE + Z_AFT_RING + Z_SPACER;

if (DRAW_AFT) {
    difference() {
        union() {
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
                cylinderKeyJoint(JUNCTION, D_AFT, D_AFT - T, 0.5);
            }
        }
        translate([0, 0, M_POMMEL_FRONT + 1])
            speakerCLS28(true);
    }
    *translate([0, 0, M_POMMEL_FRONT + Z_AFT_RING + 3]) 
        battery(D_AFT);
    *translate([0, 0, M_POMMEL_FRONT + 1])
        speakerCLS28(true);
}

if (DRAW_FRONT) {
    // Transition part; in the aft section
    translate([0, 0, M_BAFFLE_FRONT]) {
        difference() 
        {
            W = 14;
            DZ = M_TRANSITION - M_BAFFLE_FRONT;
            //tube(h=DZ, do=D_AFT, di=D_AFT - T);

            PCB_DX = 26.94;
            CUT_DX = 22.0;
            PCB_DZ = 19.32;  
            CUT_DZ = 12;
            TO_PCB = M_DISPLAY_CENTER - M_BAFFLE_FRONT - CUT_DZ / 2;
            DZ_FIX = (CUT_DZ - PCB_DZ) / 2;

            pcbHolder(  D_AFT, T, DZ,
                        TO_PCB,
                        4,
                        [CUT_DX, 50, CUT_DZ],
                        [
                            [PCB_DX/2 -  2.04, DZ_FIX + 16.01, "buttress"],
                            [PCB_DX/2 - 24.90, DZ_FIX + 16.01, "buttress"],
                            [PCB_DX/2 -  2.04, DZ_FIX +  3.31, "buttress"],
                            [PCB_DX/2 - 24.90, DZ_FIX +  3.31, "buttress"],
                        ],
                        holeAccess = true
            );
            
            // Space for the LED to slide in....
            LED_X = 19;
            translate([-LED_X/2, 0, 0]) cube(size=[LED_X, 11.5, CUT_DZ + DZ]);

            // Doesn't fit: oledHolder(D_AFT, T, DZ, 1, 7);
            cylinderKeyJoint(JUNCTION, D_AFT, D_AFT - T, 0);

            for(i=[0:2]) {
                translate([0, 0, M_DISPLAY_CENTER - M_BAFFLE_FRONT + 6 - i*6]) capsule(-160, 160);
            }
        }
    }

    D_FORWARD_INNER = dynamicHeatSinkThread();
    OVERLAP = (D_AFT - D_FORWARD_INNER) / 2 + 1;

    difference() {
        DZ = M_LED_HOLDER_FRONT - M_TRANSITION;
        union() {              
            // Front heatsink holder.
            translate([0, 0, M_LED_HOLDER_BACK]) dynamicHeatSinkHolder(D_FORWARD);

            // The front body
            translate([0, 0, M_TRANSITION]) 
                tube(h=DZ, do=D_FORWARD, di=D_FORWARD_INNER);

            // overlap ring
            translate([0, 0, M_TRANSITION - OVERLAP]) {
                difference() {
                    cylinder(h=OVERLAP, d1=D_AFT - T, d2=D_AFT);
                    cylinder(h=OVERLAP, d1=D_AFT, d2=D_FORWARD_INNER);
                }
            }
        }
        
        // Side access.
        H = 14;
        translate([0, 0, M_TRANSITION]) {
            translate([0, -H/2, 0]) {
                cube(size=[50, H, M_LED_HOLDER_BACK - M_TRANSITION]);
                mirror([1,0,0])
                // Move this up so that there is space for the dotstar connector.
                cube(size=[50, H, 31]);
            }
        }

        translate([0, 0, M_PORT_CENTER]) {
            port(true);
            portCounter();
        }

        translate([0, 0, M_SWITCH_CENTER]) {
            switch(D_FORWARD, true);
            switchCounter();
        }
    }
}

