include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>
use <case.scad>

DRAW_AFT = false;
DRAW_FORE = true;
DRAW_COUPLER = false;

T_JOINT = 3.5;
DZ_JOINT = 8;
T = 6;
EPS = 0.01;

NUT_W = 8.6;
NUT_Y = 3.4;
SMALL_NUT_W = 7.8;
SMALL_NUT_Y = 2.8;

D_ROD = 3.5;            
NUT_FLOOR = -D_INNER/2 + 3.0;

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
    M_COUPLER = M_COPPER + DZ_COPPER;
    D_HEAD = 6.6;
    H_HEAD = 2.5;
    translate([DI_COPPER/2 - D_ROD/2, 0, M_COUPLER]) cylinder(h=10, d=D_HEAD);
    translate([DI_COPPER/2 - D_ROD/2, 0, M_COUPLER-50.0]) cylinder(h=50.0+EPS, d=D_ROD);
}


module dotstarCutout()
{
    dotstarZ = dotstarStripZLen(4);
    dotstarX = 12.4;    // of the strip, not the LED

    SETBACK = 3.3;
    translate([0, 0, M_FIRST_DOTSTAR - 2.5]) mirror([-1, 0, 0]) {            
        translate([D_INNER/2 - SETBACK, 0, 0]) 
            polygonXY(h=dotstarZ, points = [
                [0, -dotstarX/2],
                [10, -dotstarX/2 + 10],
                [10, dotstarX/2 + 10],
                [0, dotstarX/2]
            ]);
        
        // Wire access    
        DY_WIRE = 7.0;
        DZ_WIRE = 4.0;
        translate([0, -DY_WIRE/2, -DZ_WIRE/2]) cube(size=[100, DY_WIRE, DZ_WIRE]);
        translate([0, -DY_WIRE/2, dotstarZ - DZ_WIRE/2]) cube(size=[100, DY_WIRE, DZ_WIRE]);
    }
}

module frontNotch()
{
    // Do something about all that plastic in the front...
    translate([-50, 6.0, M_SWITCH + 10.0]) cube(size=[100, 100, 100]);
}

if (DRAW_FORE) 
{
    H_HOLDER_0 = 3.0;
    H_HOLDER_1 = 2.0;

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
                translate([0, 0, M_COPPER]) tube(h=H_HOLDER_1, do=D_INNER, di=DO_COPPER);
            }
        }
        brassSleeve(addOuter = 1.0);
        translate([0, 0, M_ALIGN]) rotate([90, 0, 0]) cylinder(h=100, d=4.2);

        /*
        D_NUT = 9.0;
        H_NUT = 4.5;
        *translate([(DI_COPPER/2 - D_ROD/2), 0, M_COPPER - H_HOLDER_0 - H_NUT]) 
            cylinder(h=H_NUT, d=D_NUT);
        */
        // Access for rod nuts:
        translate([-50, -SMALL_NUT_W/2, M_COPPER - H_HOLDER_0 - SMALL_NUT_Y])
            cube(size=[100, SMALL_NUT_W, SMALL_NUT_Y]);
        translate([-50, -SMALL_NUT_W/2 + 2, M_COPPER - H_HOLDER_0 - SMALL_NUT_Y - 4])
            cube(size=[100, 4, SMALL_NUT_Y + 4 + EPS]);

        dotstarCutout();

        hull() {
            translate([0, 50, M_JOINT + 10.0]) rotate([90, 0, 0]) cylinder(h=100, d=12.0);
            translate([0, 50, M_JOINT + 16.0]) rotate([90, 0, 0]) cylinder(h=100, d=12.0);
        }
        for(i=[0:3])
            translate([0, 0, M_JOINT + 18.0 + i*10.0]) capsule(-110, -70, 3);
        
        // Slot for the nut.
        translate([0, 0, M_PORT]) {
            polygonXY(h=24, points=[
                [-NUT_W/2, NUT_FLOOR], [-NUT_W/2, NUT_FLOOR + NUT_Y],
                [-NUT_W/2 + 2, NUT_FLOOR + NUT_Y + 2], [NUT_W/2 - 2, NUT_FLOOR + NUT_Y + 2],
                [NUT_W/2, NUT_FLOOR + NUT_Y], [NUT_W/2, NUT_FLOOR]
            ]);
        }
        // flatten bottom
        translate([-50, -D_INNER/2 - EPS, 0]) cube(size=[100, 1.5, 200]);
        // flatten top
        translate([-50, D_INNER/2 - 1.5, 0]) cube(size=[100, 10, 200]);

        frontNotch();
    }    

    // coupler to crystal chamber
    difference() {
        union() {
            translate([0, 0, M_COPPER - H_HOLDER_0]) cylinder(d=D_INNER, h=H_HOLDER_0);
            translate([0, 0, M_COPPER]) cylinder(d=DI_COPPER, h=H_HOLDER_1);
        }
        brassSleeve(addOuter = 1.0);
        bolt();
        rotate([0, 0, 180]) bolt();

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
    }
}

module wireAccess() {
    D = 4.0;
    hull() {
        translate([(D_INNER - D)/2, 0, M_COPPER]) cylinder(h=100, d=D);
        translate([50, 0, M_COPPER]) cylinder(h=100, d=D);
    }
}

if (DRAW_COUPLER) {
    difference() {
        H_IN_COPPER = 2.0;
        echo("coupler", M_PCB_COUPLER - M_COUPLER);

        M_COUPLER = M_COPPER + DZ_COPPER;
        H_COUPLER = M_PCB_COUPLER - M_COUPLER + 2.0;

        union() {
            // end of copper to pcb
            translate([0, 0, M_COUPLER]) 
                cylinder(h=H_COUPLER, d = D_INNER, $fn=60);
            // Inside copper
            translate([0, 0, M_COUPLER - H_IN_COPPER]) 
                cylinder(h=H_COUPLER + H_IN_COPPER, d = DI_COPPER, $fn=60);
        }
        bolt();
        rotate([0, 0, 180]) bolt();
        //rotate([0, 0, 45]) wireTube();
        wireAccess();
        rotate([0, 0, 180]) wireAccess();

        // Hold the PCB
        W_PCB = 9.16 + 0.4;
        H_PCB = 14.24 + 0.4;
        translate([-W_PCB/2, -H_PCB/2, M_PCB_COUPLER]) cube(size=[W_PCB, H_PCB, 10]);
        // Punch out space under for wires & solder
        PCB_INSET = 2.0;
        translate([-W_PCB/2, -(H_PCB - PCB_INSET)/2, M_PCB_COUPLER - 2.0]) 
            cube(size=[W_PCB, H_PCB - PCB_INSET, 10]);

        // Very specific crystal holder.
        H_TIP = 1.0;
        D_TIP = 4.0;
        DX_TIP = -2.0;
        DY_TIP = 1.5;

        translate([DX_TIP, DY_TIP, M_COUPLER - H_IN_COPPER - EPS])
            cylinder(h=H_TIP, d=D_TIP);
    }
}

*color("plum") {
    translate([0, 0, M_PCB_COUPLER]) cylinder(d=D_INNER, h=10.0);
}
