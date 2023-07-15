include <dim.scad>
use <../shapes.scad>
use <../commonUnified.scad>

//$fn = 80; // overrides fs; num fragments
$fa = 6;   // minimum angle (default 12)
$fs = 0.8;  // minimum size (default 2)

N_BAFFLES = 8;
T = 4.0;
EPS = 0.01;
D_BATTERY = 19.0;
MM_TO_IN = 25.4;

module pillar(h)
{
    rotate([-90, 0, 0]) difference() {
        cylinder(d=6.0, h=h);
        translate([0, 0, -EPS]) cylinder(d=1.8, h=20);
    }
}

difference() {
    union() {
        translate([0, 0, M_START])
            difference() {
                tubeTriTop(h = DZ_SPKR_HOLDER, do = D0, di = D0 - T);
                translate([-50, -D0/2, -EPS]) cube(size=[100, D0/2 - 9.0, 100]);
            }
        translate([0, 0, M_MC])
            baffleMCBattery2(D0, N_BAFFLES, DZ_BATTERY);
        translate([0, 0, M_BOLT])
            bottomBoltRing(diameter=D0, t=T, dz=DZ_BOLT, dzToBolt=DZ_BOLT/2); 

        translate([0, 0, M_PCB]) {
            pcbHolder2(
                D0, T+2, DZ_PCB_SECTION, PCB_OFFSET, 
                [DX_PCB, DY_PILLAR_TOP, DZ_PCB],
                [
                    [22.86 / 2 - 19.685, 2.54, "buttress"],
                    [22.86 / 2 - 3.175,  2.54, "buttress"],
                    [22.86 / 2 - 19.685, 22.225, "buttress"],
                    [22.86 / 2 - 3.175,  22.225, "buttress"]
                ]
            );
        }
        // Front ring
        translate([0, 0, M_FORE_PLATE - EPS]) {
            tubeTriTop(h=3.0, do=D0, di=D0 - T);
        }   
    }

    // cutouts (thermal & wire)
    hull() {
        D = 10.0;
        translate([0, 0, M_PCB + 11.0]) rotate([90, 0, 0]) cylinder(h=100, d=D);
        translate([0, 0, M_PCB + 18.0]) rotate([90, 0, 0]) cylinder(h=100, d=D);
    } 

    // side cutouts
    for(i=[0:3]) {
        translate([-50, -10.5, M_PCB + 7.5 + i*5])
            baffleVent(2.5);
    }

    hull() {
        D = 4.0;
        translate([-50,  5, M_FORE_PLATE + 12.0]) rotate([0, 90, 0]) cylinder(h=100, d=D);
        translate([-50, -5, M_FORE_PLATE + 12.0]) rotate([0, 90, 0]) cylinder(h=100, d=D);
    } 

    hull() {
        D = 4.0;
        translate([4, 0, M_FORE_PLATE + 12.0]) rotate([-90, 0, 0]) cylinder(h=100, d=D);
        translate([-4, 0, M_FORE_PLATE + 12.0]) rotate([-90, 0, 0]) cylinder(h=100, d=D);
    }
    // flatten
    translate([-20, -D0/2 - EPS - 5.0, -20]) cube(size=[40, 0.5 + 5.0, 200]);

    // LED control
    //translate([-DX_LED_CONTROL/2, -D0/2, M_LED_CONTROL]) cube(size=[DX_LED_CONTROL, 8, DZ_LED_CONTROL]);
}

// LED pillars.
translate([0, 0, M_LED_CONTROL]) {
    Y = -7.0;
    H = 4.0;
    translate([-DX_LED_CONTROL/2 + 0.1 * MM_TO_IN, Y, 0.1 * MM_TO_IN]) pillar(h=H);
    translate([-DX_LED_CONTROL/2 + 0.1 * MM_TO_IN, Y, 1.05 * MM_TO_IN]) pillar(h=H);
    translate([-DX_LED_CONTROL/2 + 0.6 * MM_TO_IN, Y, 1.05 * MM_TO_IN]) pillar(h=H);
}

*translate([0, 0, M_FRONT]) color("red") cylinder(d=D0, h=1.0);