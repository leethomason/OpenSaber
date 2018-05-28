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
N_BAFFLES = nBafflesNeeded(H_BUTTRESS);
M_BAFFLE_FRONT = zLenOfBaffles(N_BAFFLES, H_BUTTRESS) + M_POMMEL_FRONT + EXTRA_BAFFLE;

if (DRAW_AFT) {
    translate([0, 0, M_POMMEL_FRONT]) {
        baffleMCBattery(D_AFT, N_BAFFLES, H_BUTTRESS, D_AFT_RING, 5.5, EXTRA_BAFFLE);
    }

    translate([0, 0, M_POMMEL_BACK]) {
        speakerHolder(D_POMMEL, M_POMMEL_FRONT - M_POMMEL_BACK, 2, "bass28");
    }

    translate([0, 0, M_BAFFLE_FRONT]) {
        intersection() {
            tube(JUNCTION, do=D_AFT, di=D_AFT - T);
            cylinderKeyJoint(JUNCTION - 0.5);
        }
    }
}

module heatSink()
{
    EXTRA = 0.2;
    translate([0, 0, M_LED_HOLDER_FRONT - H_HEAT_SINK_THREAD]) {
        difference() {
            cylinder(h=H_HEAT_SINK_THREAD + EXTRA, d=D_FORWARD);
            cylinder(h=H_HEAT_SINK_THREAD + EXTRA + EPS, d=D_HEAT_SINK_THREAD);
        }
    }
}


if (DRAW_FRONT) {
    heatSink();

    translate([0, 0, M_BAFFLE_FRONT]) {
        difference() {
            W = 14;
            DZ = M_TRANSITION - M_BAFFLE_FRONT;
            tube(h=DZ, do=D_AFT, di=D_AFT - T);
            //translate([-W/2, -50, 0]) {
            //    cube(size=[W, 100, DZ]);
            //}
        }
    }

    difference() {
        DZ = M_LED_HOLDER_FRONT - M_TRANSITION;
        translate([0, 0, M_TRANSITION]) tube(h=DZ, do=D_FORWARD, di=D_FORWARD - T);
        H = 14;

        translate([0, 0, M_TRANSITION]) translate([0, -H/2, 0])
            cube(size=[40, H, DZ]);

        rotate([0, 0, 90])
            translate([0, 0, M_DOTSTAR])
                dotstarLED(4, 20);    

        translate([0, 0, M_PORT_CENTER])
            port(true);

        translate([0, 0, M_SWITCH_CENTER])
            switch(D_FORWARD, true);
    }
}
