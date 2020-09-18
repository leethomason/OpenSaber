include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>
use <case.scad>

DRAW_AFT = false;
DRAW_FORE = false;
DRAW_COUPLER = true;

T_JOINT = 3.5;
DZ_JOINT = 8;
T = 6.5;
EPS = 0.01;

NUT_W = 8.6;
NUT_Y = 3.4;
SMALL_NUT_W = 8.0;
SMALL_NUT_Y = 2.8;
SMALL_NUT_D = 10.0;

D_ROD = 3.6;   // Travel free - secured by nuts
D_ROD_HEAD = 6.6;

NUT_FLOOR = -D_INNER/2 + 3.0;
COPPER_OUTSET = 0.1;

H_HOLDER_0 = 3.0;
H_HOLDER_1 = 2.0;

$fn = 40;

if (DRAW_AFT) {
    translate([0, 0, M_POMMEL]) {
        speakerHolder(D_INNER, DZ_SPKR, 3.0, "std28");
    }

    translate([0, 0, M_MC_BATTERY]) {
        baffleMCBattery(D_INNER, N_BATT_BAFFLES, DZ_BAFFLE, bridgeStyle=2);
        *color("red") battery(D_INNER, "18650");
    }

    translate([0, 0, M_JOINT]) rotate([0, 0, 0])
        keyJoint(DZ_JOINT, D_INNER, D_INNER - T_JOINT, false);

}

module bolt()
{
    M_BOLT = M_COPPER - H_HOLDER_0;
    M_HEAD = M_BOLT - 20.0;
    M_NUT = M_COPPER + DZ_COPPER;
    // Subtract 1 for access for wire
    RAD = DI_COPPER/2 - D_ROD/2 - 1.0;

    // Back access.
    translate([RAD, 0, M_HEAD]) cylinder(h=M_BOLT - M_HEAD, d=D_ROD_HEAD);
    // Bolt
    translate([RAD, 0, M_BOLT]) cylinder(h=M_NUT - M_BOLT, d=D_ROD);
}

module bolts()
{
    bolt();
    mirror([-1, 0, 0]) bolt();

    // Nut & access
    M_NUT = M_COPPER + DZ_COPPER;
    RAD = DI_COPPER/2 - D_ROD/2 - 1.0 + 3.5;
    hull() {
        translate([RAD, 0, M_NUT]) cylinder(h=100, d=SMALL_NUT_D);
        translate([-RAD, 0, M_NUT]) cylinder(h=100, d=SMALL_NUT_D);
    }
}


module dotstarCutout()
{
    dotstarZ = 30.0;
    dotstarX = 12.4;    // of the strip, not the LED
    CENTER = M_ALIGN - 7.0;

    SETBACK = 3.3;
    translate([0, 0, CENTER - dotstarZ/2]) mirror([-1, 0, 0]) {            
        translate([D_INNER/2 - SETBACK, 0, 0]) 
            polygonXY(h=dotstarZ, points = [
                [0, -dotstarX/2],
                [10, -dotstarX/2 + 10],
                [10, dotstarX/2 + 10],
                [0, dotstarX/2]
            ]);
        
        // Wire access    
        DY_WIRE = 9.0;
        DZ_WIRE = 4.0;
        translate([0, -DY_WIRE/2, -DZ_WIRE/2]) cube(size=[100, DY_WIRE, DZ_WIRE]);
        translate([0, -DY_WIRE/2, dotstarZ - DZ_WIRE/2]) cube(size=[100, DY_WIRE, DZ_WIRE]);
    }
}

module flatBottom()
{
    translate([-50, -D_INNER/2 - EPS, 0]) cube(size=[100, 2.0, 200]);
}

module frontNotch()
{
    // Do something about all that plastic in the front...
    //translate([-50, 6.0, M_SWITCH + 10.0]) cube(size=[100, 100, 100]);
    D = 2.0 * (D_INNER/2 - 6.0);

    hull() {
        translate([50, 6.0 + D/2, M_SWITCH + 18.0])
            rotate([0, -90, 0])
                cylinder(h=100, d=D);
        translate([50, 6.0 + D/2, M_SWITCH + 40])
            rotate([0, -90, 0])
                cylinder(h=100, d=D);
    }
}

if (DRAW_FORE) 
{
    DZ_POWER_RING = M_PORT - M_JOINT + 8.0;
    M_SWITCH_SECTION = M_JOINT + DZ_POWER_RING;
    DZ_SWITCH_SECTION = (M_COPPER - H_HOLDER_0) - M_SWITCH_SECTION;

    difference() {
        union() {

            // Power
            difference() {
                translate([0, 0, M_JOINT]) {
                    powerPortRing(D_INNER, T, DZ_POWER_RING, M_PORT - M_JOINT);
                }
                translate([0, 0, M_JOINT]) rotate([0, 0, 0])
                    keyJoint(DZ_JOINT, D_INNER, D_INNER - T_JOINT, true);
            }

            // Switch
            translate([0, 0, M_SWITCH_SECTION]) 
                switchRing(D_INNER, T, DZ_SWITCH_SECTION, 
                    M_SWITCH - M_SWITCH_SECTION, counter=false);

            // Bottom holder for the locking nut (punch out below)
            intersection() {
                cylinder(h=200, d=D_INNER - T + EPS);
                union() {
                    translate([-50, -D_INNER/2, M_ALIGN - 3])
                        cube(size=[100, 8, 6]);
                    translate([-50, NUT_FLOOR - 10, M_ALIGN - 3])
                        cube(size=[100, 10, 6]);
                }
            }
            // Side holder for locking nuts
            intersection() 
            {
                cylinder(h=200, d=D_INNER - T + EPS);
                union() {
                    translate([6.0, -50, M_COPPER - H_HOLDER_0 - 5.0]) 
                        cube(size=[100, 100, 5.0 + EPS]);
                    mirror([-1, 0, 0]) translate([6.0, -50, M_COPPER - H_HOLDER_0 - 5.0]) 
                        cube(size=[100, 100, 5.0 + EPS]);
                }
            }

            // Lock in the copper
            intersection() 
            {
                union() {
                    translate([-50, -D_INNER/2, M_COPPER]) cube(size=[45, 12.0, H_HOLDER_1]);
                    mirror([-1, 0, 0]) translate([-50, -D_INNER/2, M_COPPER]) cube(size=[45, 12.0, H_HOLDER_1]);
                }
                translate([0, 0, M_COPPER]) 
                    tube(h=H_HOLDER_1, do=D_INNER, di=DO_COPPER + COPPER_OUTSET);
            }
        }
        brassSleeve(addOuter = 1.0);
        translate([0, 0, M_ALIGN]) rotate([90, 0, 0]) cylinder(h=100, d=4.2);

        bolts();

        dotstarCutout();

        hull() {
            translate([0, 50, M_JOINT + 10.0]) rotate([90, 0, 0]) cylinder(h=100, d=12.0);
            translate([0, 50, M_JOINT + 16.0]) rotate([90, 0, 0]) cylinder(h=100, d=12.0);
        }
        for(i=[0:3])
            translate([0, 0, M_JOINT + 18.0 + i*10.0]) capsule(-110, -70, 3);
        
        // Slot for the bottom nut.
        translate([0, 0, M_PORT]) {
            polygonXY(h=24, points=[
                [-NUT_W/2, NUT_FLOOR], [-NUT_W/2, NUT_FLOOR + NUT_Y],
                [-NUT_W/2 + 2, NUT_FLOOR + NUT_Y + 2], [NUT_W/2 - 2, NUT_FLOOR + NUT_Y + 2],
                [NUT_W/2, NUT_FLOOR + NUT_Y], [NUT_W/2, NUT_FLOOR]
            ]);
        }
        // flatten bottom
        flatBottom();
        // flatten top
        translate([-50, D_INNER/2 - 1.5, 0]) cube(size=[100, 10, 200]);

        frontNotch();
    }    

    // coupler to crystal chamber
    difference() {
        union() {
            translate([0, 0, M_COPPER - H_HOLDER_0]) cylinder(d=D_INNER, h=H_HOLDER_0);
            translate([0, 0, M_COPPER]) cylinder(d=DI_COPPER - COPPER_OUTSET, h=H_HOLDER_1);
        }
        brassSleeve(addOuter = 1.0);
        bolts();

        // Cut the dotstar holder:
        DOTSIZE = 5.2;
        STRIPSIZE = 13.0;
        STRIPTHICK = 1.2;
        WIRESIZE = 10.0;
        translate([-DOTSIZE/2, -DOTSIZE/2, M_COPPER - H_HOLDER_0 - EPS])
            cube(size=[DOTSIZE, 100, 10]);            
        translate([-STRIPSIZE/2, -4, M_COPPER - 1.5])
            cube(size=[STRIPSIZE, 100, STRIPTHICK]);
        translate([-WIRESIZE/2, 2.0, M_COPPER - H_HOLDER_0 - EPS])
            cube(size=[WIRESIZE, 100, 8.0]);            

        frontNotch();
        flatBottom();
    }
}

module wireAccess() {
    *translate([D_INNER/2 - 5, -50, M_COPPER + DZ_COPPER])
        cube(size=[50, 52, 100]);
    translate([D_INNER/2 - 4, -10, M_COPPER + DZ_COPPER])
        cylinder(h=100, d=7);

}

if (DRAW_COUPLER) {
    difference() {
        H_IN_COPPER = 2.5;
        echo("coupler", M_PCB_COUPLER - M_COUPLER);

        M_COUPLER = M_COPPER + DZ_COPPER;
        H_COUPLER = M_PCB_COUPLER - M_COUPLER + 2.0;

        union() {
            // end of copper to pcb
            translate([0, 0, M_COUPLER]) 
                cylinder(h=H_COUPLER + 0.5, d = D_INNER, $fn=60);
            // Inside copper
            translate([0, 0, M_COUPLER - H_IN_COPPER]) 
                cylinder(h=H_COUPLER + H_IN_COPPER, d = DI_COPPER, $fn=60);
        }
        bolts();
        wireAccess();
        mirror([-1, 0, 0]) wireAccess();

        // Hold the PCB
        W_PCB = 9.16 + 0.4;
        H_PCB = 16.78 + 0.4;
        translate([-W_PCB/2, -H_PCB/2, M_PCB_COUPLER + 1.0]) cube(size=[W_PCB, H_PCB, 10]);
        // Punch out space under for wires & solder
        PCB_INSET = 2.5;
        translate([-W_PCB/2, -(H_PCB - PCB_INSET)/2, M_PCB_COUPLER - 2.8]) 
            cube(size=[W_PCB, H_PCB - PCB_INSET, 10]);

        // Very specific crystal holder.
        D_TIP = 8.0;
        DX_TIP = 0.0;
        DY_TIP = 0.0;

        translate([DX_TIP, DY_TIP, M_COUPLER - H_IN_COPPER - EPS])
            cylinder(h=10, d=D_TIP);
    }
}

*color("plum") {
    translate([0, 0, M_PCB_COUPLER]) cylinder(d=D_INNER, h=10.0);
}
