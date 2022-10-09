use <../commonUnified.scad>
use <../shapes.scad>
use <../inset.scad>
include <dim.scad>

DRAW_AFT = true;
DRAW_FORE = false;

ARC_INNER_DX = INNER_DX / 2;
ARC_INNER_DY = (INNER_ARCY - INNER_DY) / 2;

INNER_R = (ARC_INNER_DX * ARC_INNER_DX + ARC_INNER_DY * ARC_INNER_DY) / (2 * ARC_INNER_DY);
//echo("INNER_R", INNER_R);
SWITCH_Y_OFFSET = INNER_R - yAtX(POWER_DX/2, INNER_R);
SWITCH_Y = INNER_ARCY/2 - SWITCH_Y_OFFSET - 1.5;    // 1.5 is for the switch to clear the carriage insert
//echo("SWITCH_Y_OFFSET", SWITCH_Y_OFFSET);

INNER_SCALE = 0.8;
EPS = 0.01;

M_WINDOW_1 = M_BOLT_2 - 7.0;
M_WINDOW_0 = M_BOLT_1 + 10.0;
WINDOW_ADD = 6.0;
WINDOW_CUT = 1.0;

D_BATTERY           = 18.50 + 0.5;
Z_BATTERY_18500     = 70.0;
M_BATTERY = M_SWITCH_SECTION_START - Z_BATTERY_18500;

MC_DROP = 12.0;

$fn = 80;

module railJoint()
{
    W = 5.0;
    C = 10.0;
    L = 8.0;

    translate([-W/2, -100, 0]) cube(size=[W, 100, L]);
    translate([0, 0, L]) rotate([90, 0, 0]) cylinder(h=100, d=C);
}

module arcCylinder(h, dx, dy)
{
    r = (dx*dx + dy*dy) / (2 * dy);

    intersection() {
        translate([-dx, 0, 0]) cube(size=[dx*2, dy, h]);
        translate([0, dy - r, 0]) cylinder(h=h, r=r);
    }
}

module windowAdd()
{
    T = 4.0;
    translate([INNER_DX/2 - T, -50, -WINDOW_ADD/2]) cube(size=[10, 100, WINDOW_ADD]);
    mirror([-1, 0, 0]) translate([INNER_DX/2 - T, -50, -WINDOW_ADD/2]) cube(size=[10, 100, WINDOW_ADD]);
}

module windowCut()
{
    translate([-50, 0, 0]) polygonYZ(h=100, points=[
        [-8, -WINDOW_CUT],
        [-8 + WINDOW_CUT*2, WINDOW_CUT],
        [8, WINDOW_CUT],
        [8 - + WINDOW_CUT*2, -WINDOW_CUT]
    ]);
}

module inner(extend=0)
{
    difference() {
        union() {
            translate([0, INNER_DY/2 - EPS, M_CARRIAGE - extend/2])
                arcCylinder(h=DZ_CARRIAGE + extend, dx=INNER_DX / 2, dy=(INNER_ARCY - INNER_DY)/2);
            mirror([0, -1, 0])
                translate([0, INNER_DY/2 - EPS, M_CARRIAGE - extend/2])
                    arcCylinder(h=DZ_CARRIAGE + extend, dx=INNER_DX / 2, dy=(INNER_ARCY - INNER_DY)/2);
            translate([-INNER_DX/2, -INNER_DY/2, M_CARRIAGE - extend/2])
                cube(size=[INNER_DX, INNER_DY, DZ_CARRIAGE + extend]);
        }
        // hint of flattening
        translate([-50, -INNER_ARCY/2, 0]) cube(size=[100, 0.5, 1000]);
    }
}

// z=0 is the pin top
module power(zExtend)
{
    Z_BACK = POWER_DZ - POWER_DZ_PIN;
    translate([-POWER_DX/2, -POWER_DY, -Z_BACK - zExtend])
        cube(size=[POWER_DX, 100, POWER_DZ + zExtend]);
}

module switchArm()
{
    PIT = 20.0;
    translate([SWITCH_DX/2, -PIT, (SWITCH_DZ - SWITCH_ARM_DZ)/2]) 
        cube(size=[SWITCH_ARM, 100, SWITCH_ARM_DZ]);
}

// top
module switch()
{
    switchArm();
    mirror([-1, 0, 0]) switchArm();

    translate([-SWITCH_DX/2, -SWITCH_DY, 0]) cube(size=[SWITCH_DX, 100, SWITCH_DZ]);
}

module carriage()
{
    difference() {
        inner();
        scale([INNER_SCALE, INNER_SCALE, 1]) inner(1);

        for(i=[1:5]) {
            translate([0, 0, M_CARRIAGE + 0 + i*20]) hull() {
                S = 10;
                translate([-50, 5, 0]) rotate([0, 90, 0]) cylinder(h=100, d=S);
                translate([-50, 50, -S/2]) cube(size=[100, 1, S]);
            }
        }
        W = 4; 
        H = 10;
        for(i=[1:5]) {
            //DY = (i <= 2) ? -2.4 : 0;
            DY = 0;
            translate([-50, -10 + DY, M_CARRIAGE + 10 + i*20])
                polygonZY(h=100, points=[
                    [-W/2, 0], [W/2, 0], [W/2, H-W], [0, H], [-W/2, H-W]
                ]);
        }
    }
}

module diamondBolt(d, dz)
{
    r = d/2;
    polygonXY(h=dz, points=[
        [0, -r], [r, 0], [0, r], [-r, 0]
    ]);
}

module bat(extend)
{
    color("red") 
        translate([0, INNER_ARCY/2 - D_BATTERY / 2, 0])
            cylinder(h=Z_BATTERY_18500 + extend, d=D_BATTERY);
}

module buttress(extraY)
{
    DX = 5.0;
    Y = 5.0;
    translate([INNER_DX/2 - DX, -INNER_ARCY/2, 0]) cube(size=[12, INNER_ARCY/2 + Y, 3.0]);
    mirror([-1, 0, 0]) translate([INNER_DX/2 - DX, -INNER_ARCY/2, 0]) cube(size=[12, INNER_ARCY/2 + Y, 3.0]);

    if (extraY) {
        polygonXY(h=3.0, points=[
            [-INNER_DX/2 + DX, Y], [1, 14],
            [-INNER_DX, 14], [-INNER_DX, Y]
        ]);
        mirror([-1, 0, 0]) polygonXY(h=3.0, points=[
            [-INNER_DX/2 + DX, Y], [1, 14],
            [-INNER_DX, 14], [-INNER_DX, Y]
        ]);
    }
}

if (DRAW_FORE) {
    BOLT_Y = INNER_ARCY/2 - 3.0;

    difference() {
        union() {
            // Outer hull section.
            intersection() {
                difference() {
                    inner();
                    scale([INNER_SCALE, INNER_SCALE, 1]) inner(1);

                    // bottom access
                    hull() {
                        D = 14.0;
                        translate([0, 0, M_WINDOW_0 + D/2]) rotate([90, 0, 0]) cylinder(h=100, d=D);
                        translate([0, 0, M_WINDOW_1 - D/2]) rotate([90, 0, 0]) cylinder(h=100, d=D);
                    }
                }
                translate([-50, -50, M_SWITCH_SECTION_START]) cube(size=[100, 100, DZ_SWITCH_SECTION]);

            }
            intersection() {
                inner();

                // Add material to hold switch
                union() {
                    translate([-50, -2, M_SWITCH - 6]) {
                        cube(size=[100, 100, 14]);
                    }
                    BOLT_PAD = 6.0;
                    // flat part for bolts.
                    translate([-50, BOLT_Y, M_BOLT_1 - BOLT_PAD/2]) cube(size=[100, 100, BOLT_PAD]);
                    translate([-50, BOLT_Y, M_BOLT_2 - BOLT_PAD/2]) cube(size=[100, 100, BOLT_PAD]);                        

                    translate([0, 0, M_WINDOW_1]) windowAdd();
                    translate([0, 0, M_WINDOW_0]) windowAdd();
                }
            }
        }
        // use a hull so the finger has space in front of the switch to press it.
        hull() {
            translate([0, SWITCH_Y, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=10, d=D_HOLE);
            translate([0, SWITCH_Y, M_SWITCH + 5.0]) rotate([-90, 0, 0]) cylinder(h=10, d=D_HOLE);
        }
        // knock out a chunk behind the power port so it can be installed.
        translate([0, SWITCH_Y, ACTUAL_POWER_PIN_Z]) power(7);
        translate([0, SWITCH_Y, ACTUAL_POWER_PIN_Z + POWER_DZ_PIN]) switch();

        // Knock out the bolts.
        translate([0, 0, M_BOLT_1]) rotate([-90, 0, 0]) cylinder(h=20, d=D_BOLT);
        translate([0, 0, M_BOLT_2]) rotate([-90, 0, 0]) cylinder(h=20, d=D_BOLT);

        translate([0, 0, M_WINDOW_1]) windowCut();
        translate([0, 0, M_WINDOW_0]) windowCut();

        translate([0, 0, M_SWITCH_SECTION_START-EPS]) railJoint();

    }

    intersection() {
        inner();
        union() {
            translate([0, 0, M_BOLT_1]) boltRingNoShell(BOLT_Y, bolt_d=D_BOLT, nut_w=5.5, nut_y=2.5);
            translate([0, 0, M_BOLT_2]) boltRingNoShell(BOLT_Y, bolt_d=D_BOLT, nut_w=5.5, nut_y=2.5);
        }
    }
}

if (DRAW_AFT) {
    Y_MC = 9.0;
    MC_DY = INNER_ARCY / 2 - D_BATTERY - Y_MC;

    difference() {
        union() {
            carriage();
            intersection() {
                scale([0.95, 0.95, 1]) inner(1);
                for(i=[0:23]) {
                    EXTRA = (i==0) ? 100 : 0;
                    translate([0, 0, M_CARRIAGE + EPS + i * 5.1]) 
                        buttress(extraY=(i < 3) || (i==5) || (i==6) || (i==9) || (i==10));
                }
            }
        }

        difference() {
            translate([-50, -50, M_SWITCH_SECTION_START]) cube(size=[100, 100, 1000]);
            translate([0, 0, M_SWITCH_SECTION_START]) railJoint();
        }

        translate([PCB_BOLT_DX/2, PCB_BOLT_DY, M_CARRIAGE - EPS]) diamondBolt(dz=6.0, d=PCB_BOLT_D_DIAMOND);
        translate([-PCB_BOLT_DX/2, PCB_BOLT_DY, M_CARRIAGE - EPS]) diamondBolt(dz=6.0, d=PCB_BOLT_D_DIAMOND);

        translate([0, MC_DY, M_CARRIAGE - EPS]) mc();
        {
            X_MC = 0.7 * 25.4;
            Y_MC = 9.0;
            Z_MC = 2.2 * 25.4 + 0.1;
            MC_T = 0.5;
        }
        translate([-X_MC/2, MC_DY - 20, M_CARRIAGE - EPS]) cube(size=[X_MC, 20 + EPS, MC_DROP]);
        translate([-X_MC/2, MC_DY - 20, M_CARRIAGE - EPS]) cube(size=[MC_T, 20 + EPS, Z_MC]);
        mirror([-1, 0, 0]) translate([-X_MC/2, MC_DY - 20, M_CARRIAGE - EPS]) cube(size=[MC_T, 20 + EPS, Z_MC]);

        translate([0, 0, M_BATTERY]) bat(10);

        CUT_X = 18.0;
        translate([-CUT_X/2, 2.0, M_BATTERY]) cube(size=[CUT_X, 100, 100]);

        // arms above the battery just fall off.
        translate([-50, 6, M_BATTERY + 5]) cube(size=[100, 100, 45]);

        translate([-50, 0, M_CARRIAGE-EPS]) polygonZY(h=100, points=[
            [10, -13], [0, -3], [0, -13]
        ]);

        // Lots and lots and lots of thermal problems with this part.
        translate([-50, -12.5, M_CARRIAGE-EPS])
            cube(size=[100, 3, 51]);

        // Oops - the "tongue" clicks the reset button.
        translate([0, 0, M_RESET]) rotate([90, 0, 0])
            cylinder(h=50, d=5);

    }
    *translate([0, MC_DY, M_CARRIAGE]) mc();
    *translate([0, 0, M_BATTERY]) bat(0);
}

*color("red") {
    translate([0, INNER_ARCY/2, M_BOLT_1]) rotate([-90, 0, 0]) cylinder(h=4, d=D_BOLT);
    *translate([0, INNER_ARCY/2, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=4, d=D_SWITCH);
    translate([0, INNER_ARCY/2, M_BOLT_2]) rotate([-90, 0, 0]) cylinder(h=4, d=D_BOLT);
    translate([0, INNER_ARCY/2, M_BOLT_3]) rotate([-90, 0, 0]) cylinder(h=4, d=D_BOLT);
}