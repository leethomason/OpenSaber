use <../shapes.scad>
include <dim.scad>
use <../commonUnified.scad>

$fn = 90;
EPS = 0.01;
EPS2 = EPS * 2;

DRAW_AFT = false;
DRAW_FRONT = true;

D_HEAT_SINK_THREAD = 20.2;  // 20.4 is loose (PHA), 20.1 tight (PLA)

T = 4;
JUNCTION = 6;
EXTRA_BAFFLE = 2;
N_BAFFLES = nBafflesNeeded(H_BUTTRESS);
M_BAFFLE_FRONT = zLenOfBaffles(N_BAFFLES, H_BUTTRESS) + M_POMMEL_FRONT + EXTRA_BAFFLE;

if (DRAW_AFT) {
    translate([0, 0, M_POMMEL_FRONT]) {
        baffleMCBattery(D_AFT, N_BAFFLES, H_BUTTRESS, D_AFT_RING, 5.5, EXTRA_BAFFLE);
    }

    *translate([0, 0, M_POMMEL_BACK]) {
        speakerHolder(D_POMMEL, M_POMMEL_FRONT - M_POMMEL_BACK, 2, "bass28");
    }

    translate([0, 0, M_BAFFLE_FRONT]) {
        cylinderKeyJoint(JUNCTION, D_AFT, D_AFT - T, 0.5);
    }
}


module heatSink()
{
    DZ = M_LED_HOLDER_FRONT - M_LED_HOLDER_BACK;
    translate([0, 0, M_LED_HOLDER_BACK]) {
        difference() {
            cylinder(h=DZ, d=D_FORWARD);
            cylinder(h=DZ + EPS, d=D_HEAT_SINK_THREAD);
        }
    }
}


if (DRAW_FRONT) {
    // Transition part; in the aft section
    translate([0, 0, M_BAFFLE_FRONT]) {
        difference() 
        {
            W = 14;
            DZ = M_TRANSITION - M_BAFFLE_FRONT;
            tube(h=DZ, do=D_AFT, di=D_AFT - T);
            // Doesn't fit: oledHolder(D_AFT, T, DZ, 1, 7);
            cylinderKeyJoint(JUNCTION, D_AFT, D_AFT - T, 0);
        }
    }

    // The adjustment to the inner radius
    // is so the dotstars can fit beside the heat
    // sink thread. But since the dotstars live in 
    // an inset strip now, this can be larger.
    D_FORWARD_INNER = D_FORWARD - T;

    OVERLAP = 4;

    difference() {
        DZ = M_LED_HOLDER_FRONT - M_TRANSITION;
        union() {              
            // Front heatsink holder.
            heatSink();

            // The front body
            translate([0, 0, M_TRANSITION]) 
                tube(h=DZ, do=D_FORWARD, di=D_FORWARD_INNER);

            // overlap ring
            translate([0, 0, M_TRANSITION - OVERLAP]) {
                tube(h=OVERLAP, do=D_AFT, di=D_FORWARD - T);
            }
        }
        
        // Side access.
        H = 14;
        translate([0, 0, M_TRANSITION - OVERLAP]) {
            translate([0, -H/2, 0]) {
                cube(size=[50, H, M_LED_HOLDER_BACK - M_TRANSITION + OVERLAP]);
                mirror([1,0,0])
                // Move this up so that there is space for the dotstar connector.
                cube(size=[50, H, 31 + OVERLAP]);
            }
        }

        rotate([0, 0, 90])
            translate([0, 0, M_DOTSTAR]) {
                dotstarLED(4, 20);    
                dotstarStrip(4, 0, 11.5);
            }

        translate([0, 0, M_PORT_CENTER])
            port(true);

        translate([0, 0, M_SWITCH_CENTER])
            switch(D_FORWARD, true);
    }
}

