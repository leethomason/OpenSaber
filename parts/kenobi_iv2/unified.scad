include <dim.scad>
use <../shapes.scad>
use <../commonUnified.scad>

$fn = 40;
N_BAFFLES = 8;
T = 4.0;
EPS = 0.01;

module inner()
{
    translate([0, 0, M0]) cylinder(h=DZ_AFT, d=D0);
    translate([0, 0, M1]) cylinder(h=50, d=D1);
}

translate([0, 0, M0])
    speakerHolder(D0, DZ_SPKR_HOLDER, 1.5, "cls28");
translate([0, 0, M_MC])
    baffleMCBattery(D0, N_BAFFLES, DZ_BATTERY, bridgeStyle=3);

*translate([0, 0, M_MC]) color("red") battery(D0, "18650");

difference() {
    union() {
        translate([0, 0, M_PCB]) {
            DZ_SECTION = DZ_PCB;
            pcbHolder2(
                D0, T, DZ_SECTION, 0.0, [23.0, -1.0, 25.0],
                [
                    [22.86 / 2 - 19.685, 2.54, "buttress"],
                    [22.86 / 2 - 3.175,  2.54, "buttress"],
                    [22.86 / 2 - 19.685, 22.225, "buttress"],
                    [22.86 / 2 - 3.175,  22.225, "buttress"]
                ]
            );
        }
        intersection() {
            inner();
            translate([0, 0, M_FORE_PLATE - EPS]) {
                LEN = 20.0;
                difference() {
                    cylinder(h=LEN, d=D0);
                    cylinder(h=200, d=D1 - T*2);
                }
            }   
        }
    }
    // Make sure the battery can be inserted.
    D_BATTERY = 19.0;
    color("yellow") hull() {
        translate([0, D0/2 - D_BATTERY/2, 0]) cylinder(d=D_BATTERY, h = 200.0);
        translate([0, 50, 0]) cylinder(d=D_BATTERY, h = 200.0);
    }

    // cutouts (thermal & wire)
    hull() {
        D = 10.0;
        translate([0, 0, M_PCB + 11.0]) rotate([90, 0, 0]) cylinder(h=100, d=D);
        translate([0, 0, M_FORE_PLATE + 6.0]) rotate([90, 0, 0]) cylinder(h=100, d=D);
    }

    // flatten
    translate([-20, -D0/2 - EPS, -20]) cube(size=[40, 1.0, 200]);
}