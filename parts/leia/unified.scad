use <../commonUnified.scad>
use <../shapes.scad>
use <../inset.scad>
include <dim.scad>

DRAW_FORE = true;
DRAW_AFT = false;

EPS = 0.01;
EPS2 = EPS*2;

N_BATTERY_BAFFLES = nBafflesNeeded(DZ_BUTTRESS, "18650");
N_TOTAL_BAFFLES = floor(DZ_FORE / (DZ_BUTTRESS * 2)) - 1;
N_POST_BAFFLES = N_TOTAL_BAFFLES - N_BATTERY_BAFFLES;

TRANSITION_DZ = 8.0;
TRANSITION_CUTOUT = 10.0;

module drawFore()
{
    translate([0, 0, M_FORE]) {
        baffleMCBattery(D_INNER_FORE,
            N_BATTERY_BAFFLES,
            DZ_BUTTRESS,
            nPostBaffles=N_POST_BAFFLES,
            bridgeStyle=1);

        difference() {
            translate([0, 0, -TRANSITION_DZ]) {
                tube(h=TRANSITION_DZ, do=D_INNER_AFT, di=D_INNER_FORE-4, $fn=80);
            }
            translate([-TRANSITION_CUTOUT/2, -50, -TRANSITION_DZ - EPS]) {
                cube(size=[TRANSITION_CUTOUT, 100, TRANSITION_DZ + EPS2]);
            }
            translate([0, 0, -TRANSITION_DZ]) keyJoint(TRANSITION_DZ, D_INNER_AFT, D_INNER_AFT - 4, true);
        }
    }
    // Connect up.
    for(i=[1:2]) {
        translate([0, 0, M_FORE + zLenOfBaffles(N_TOTAL_BAFFLES, DZ_BUTTRESS) - (i * 2 + 1) * DZ_BUTTRESS]) {
            translate([-10, 3, 0])
                cube(size=[20, 7.5, DZ_BUTTRESS]);
        }
        translate([0, 0, M_FORE + zLenOfBaffles(N_TOTAL_BAFFLES, DZ_BUTTRESS) - DZ_BUTTRESS]) {
            tube(h=DZ_BUTTRESS, do=D_INNER_FORE, di=D_INNER_FORE - 6);
        }
    }
}

module drawAft()
{
    translate([0, 0, 20]) {
        DZ = M_T - 20 - TRANSITION_DZ; 
        DZ_START = 20.0;
        DZ_END = DZ - 5.0;
        DZ_BOLT = (DZ_START + DZ_END) / 2;
        DZ_PORT = (DZ_START + DZ_END) / 2 - 10.5;
        DZ_SWITCH = (DZ_START + DZ_END) / 2 + 10.5;

        insetHolder(
            D_INNER_AFT,
            D_OUTER_AFT,
            14.0,
            DZ,
            DZ_BUTTRESS,
            DZ_START, 
            DZ_END,     
            DZ_BOLT,  
            DZ_PORT,
            DZ_SWITCH,
            roundRect = 3.175/2,
            frontBridge=false
        );
    }
    translate([0, 0, 2]) {
        speakerHolder(D_INNER_AFT, 18, 3, "std28");
    }
    translate([0, 0, M_FORE - TRANSITION_DZ]) keyJoint(TRANSITION_DZ, D_INNER_AFT, D_INNER_AFT - 4, false);
}

if (DRAW_FORE) drawFore();
if (DRAW_AFT) drawAft();

//color("red") battery(D_INNER_FORE, "18650");
//color("green") mc();
