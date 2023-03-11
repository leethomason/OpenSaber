include <dim.scad>
use <../shapes.scad>
use <../commonUnified.scad>

//$fn = 80; // overrides fs; num fragments
$fa = 6;   // minimum angle (default 12)
$fs = 0.8;  // minimum size (default 2)

T = 6.5;
JOINT = 8;
KEYJOINT_T = 5.0;
D_PORT =  7.9;
EPS = 0.01;

DRAW_AFT = true;
DRAW_FORE = true;
DRAM_EMITTER = false;

module flatBottom()
{
    translate([-50, -D0/2 - 10, -10]) 
        cube(size=[100, 10.0 + 0.8, 1000]);
}

if (DRAW_AFT) {
    translate([0, 0, M1])
        speakerHolder(D0, DZ_SPKR_HOLDER, 3.0, "cls28");
    translate([0, 0, M_MC])
        baffleMCBattery(D0, N_BAFFLES, DZ_BATTERY, bridgeStyle=5, thermalRelief=5);
    translate([0, 0, M_SECTION0])
        keyJoint(JOINT, D0, D0 - KEYJOINT_T, false);
}

if (DRAW_FORE) color("cadetblue") {
    difference() {
        union() {
            translate([0, 0, M2 - DZ_HEATSINK_CASE - DZ_HEATSINK_THREAD])
                dynamicHeatSinkHolder(D0);

            color("silver") translate([0, 0, M_SECTION0])
                tube(h=DZ_SECTION0 + EPS, di=D0 - T, do=D0);
            color("white") translate([0, 0, M_HEATSINK])
                emitterTeeth(D0);

            // Build up port:
            S_PORT = 10.0;
            intersection() {
                cylinder(h=1000, d=D0);
                rotate([0, 0, ROT_PORT]) translate([-S_PORT/2, D0/2 - 4.8, M_PORT - S_PORT/2]) cube(size=[S_PORT, 10, S_PORT]);
            }
        }        
        translate([0, 0, M_SECTION0 - EPS])
            keyJoint(JOINT, D0, D0 - KEYJOINT_T, true);

        // Switch 
        translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=100, d=17.0 + 1.0);
        // Alignment
        translate([0, 0, M_BOLT0]) rotate([-90, 0, 0]) cylinder(h=100, d=3.5);
        translate([0, 0, M_BOLT1]) rotate([-90, 0, 0]) cylinder(h=100, d=3.5);
        // Port
        translate([0, 0, M_PORT]) rotate([-90, 0, ROT_PORT]) cylinder(h=100, d=D_PORT);
        // Installation for port:
        translate([0, 0, M_PORT]) rotate([90, 0, ROT_PORT]) cylinder(h=100, d=16.0);

        // vents
        for(i=[0:2]) 
            translate([0, 0, M_SECTION0 + 20 + 10 * i]) triCapsule(60, 120, 2, true);
        for(i=[0:1]) {
            translate([0, 0, M_SECTION0 + 50 + 10 * i]) triCapsule(180-90, 180-120, 2, false);
            translate([0, 0, M_SECTION0 + 50 + 10 * i]) triCapsule(240, 260, 2, false);
        }

        // bottom
        hull() {
            translate([0, 0, M_SECTION0 + 20]) rotate([90, 0, 0]) cylinder(h=100, d=16);
            translate([0, 0, M_SECTION0 + 40]) rotate([90, 0, 0]) cylinder(h=100, d=16);
        }

        // back top
        hull() {
            translate([-3, 0, M_SECTION0 + 10]) rotate([-90, 0, 0]) cylinder(h=100, d=12);
            translate([ 3, 0, M_SECTION0 + 10]) rotate([-90, 0, 0]) cylinder(h=100, d=12);
        }

        // front top
        hull() {
            translate([-3, 0, M_SECTION0 + 65]) rotate([-90, 0, 0]) cylinder(h=100, d=6);
            translate([ 3, 0, M_SECTION0 + 65]) rotate([-90, 0, 0]) cylinder(h=100, d=6);
        }
        // front bottom
        hull() {
            translate([-3, 0, M_SECTION0 + 65]) rotate([90, 0, 0]) cylinder(h=100, d=6);
            translate([ 3, 0, M_SECTION0 + 65]) rotate([90, 0, 0]) cylinder(h=100, d=6);
        }

        flatBottom();
    }
}

if (DRAM_EMITTER) {

    difference() {
        translate([0, 0, M_HEATSINK]) tube(h=DZ_HEATSINK_CASE, do=D0, di=D_HEATSINK_CASE);
        translate([0, 0, M_HEATSINK - EPS]) emitterTeeth(D0, 0.2, true);

        Z0 = 5.0;
        Z1 = 14.0;
        Z2 = 18.0;

        translate([0, 0, M_HEATSINK]) for(r=[0:5]) {
            rotate([0, 0, r*60])
                polygonXZ(h=50, points=
                [
                    [-5, Z2],
                    [5, Z2],
                    [5, Z1],
                    [0, Z0],
                    [-5, Z1]
                ]);
        }
    }

}
