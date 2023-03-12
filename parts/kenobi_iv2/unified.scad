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

module ringLock()
{
    difference() {
        translate([0, 0, M_AL + 10.1]) {
            tube(h=5.0, di=D0, do=34.9  - 1.3);
        }
        W = 22.0;
        translate([-W/2, 0, 0]) {
            cube(size=[W, 100, 100]);
        }
    }
}

module inner()
{
    translate([0, 0, M0]) cylinder(h=DZ_AFT, d=D0);
    translate([0, 0, M1]) cylinder(h=50, d=D1);
    //ringLock();
}

difference() {
    union() {
        translate([0, 0, M0])
            speakerHolder(D0, DZ_SPKR_HOLDER, 1.5, "cls28");
        translate([0, 0, M_MC])
            baffleMCBattery2(D0, N_BAFFLES, DZ_BATTERY);
            //baffleMCBattery(D0, N_BAFFLES, DZ_BATTERY, bridgeStyle=5, thermalRelief=3);
    }
    // EXPERIMENT drop in battery
    translate([0, 0, M_MC]) {
        hull() {
            translate([0, D0/2 - D_BATTERY/2, 0]) cylinder(d=D_BATTERY, h = DZ_BATTERY+EPS);
            translate([0, 50, 0]) cylinder(d=D_BATTERY, h = DZ_BATTERY+EPS);
        }
    }
}

*translate([0, 0, M_MC]) color("red") battery(D0, "18650");

difference() {
    union() {
        translate([0, 0, M_PCB]) {
            DZ_SECTION = DZ_PCB;
            pcbHolder2(
                D0, T, DZ_SECTION, -EPS, 
                [DX_PCB, DY_PILLAR_TOP, DZ_PCB],
                [
                    [22.86 / 2 - 19.685, 2.54, "buttress"],
                    [22.86 / 2 - 3.175,  2.54, "buttress"],
                    [22.86 / 2 - 19.685, 22.225, "buttress"],
                    [22.86 / 2 - 3.175,  22.225, "buttress"]
                ]
            );

            // Lip to hold pcb
            intersection() {
                cylinder(h=100, d=D0);
                difference() {
                    union() {
                        translate([DX_PCB/2 - 1.0, -D0/2, 0])
                            cube(size=[10, D0, DZ_SECTION]);
                        mirror([-1, 0, 0]) translate([DX_PCB/2 - 1.0, -D0/2, 0])
                            cube(size=[10, D0, DZ_SECTION]);
                    }
                    translate([-DX_PCB/2, DY_PCB_BOTTOM, 0])
                        cube(size=[DX_PCB, 100, DZ_PCB]);
                }
            }
        }
        // Front tube
        intersection() {
            inner();
            FRONT_T = 8;
            translate([0, 0, M_FORE_PLATE - EPS]) {
                LEN = 20.0;
                difference() {
                    cylinder(h=LEN, d=D0);
                    translate([0, 0, -EPS]) cylinder(h=200, d=D1 - FRONT_T);
                }
            }   
        }

        // Ring lock
        ringLock();
    }

    // EXPERIMENT drop in battery
    // Make sure the battery can be inserted.
    *color("yellow") hull() {
        translate([0, D0/2 - D_BATTERY/2, 0]) cylinder(d=D_BATTERY, h = 200.0);
        translate([0, 50, 0]) cylinder(d=D_BATTERY, h = 200.0);
    }

    // cutouts (thermal & wire)
    hull() {
        D = 10.0;
        translate([0, 0, M_PCB + 11.0]) rotate([90, 0, 0]) cylinder(h=100, d=D);
        translate([0, 0, M_FORE_PLATE + 6.0]) rotate([90, 0, 0]) cylinder(h=100, d=D);
    } 

    // side cutouts
    hull() {
        D = 4.0;
        translate([-50, -7, M_PCB + 9.0]) rotate([0, 90, 0]) cylinder(h=100, d=D);
        translate([-50, -7, M_PCB + 16.0]) rotate([0, 90, 0]) cylinder(h=100, d=D);
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

    // set pin
    translate([0, 0, M_SET_PIN]) rotate([0, 90, 0]) cylinder(h=100, d=4);
    translate([0, 0, M_SET_PIN]) rotate([0, -90, 0]) cylinder(h=100, d=4);

    // flatten
    translate([-20, -D0/2 - EPS - 5.0, -20]) cube(size=[40, 0.5 + 5.0, 200]);
}

