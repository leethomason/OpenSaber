include <dim.scad>
use <../shapes.scad>
use <../commonUnified.scad>
use <../inset.scad>

DRAW_AFT = false;
DRAW_FORE = true;
DRAW_EMITTER = false;

$fn=60;
EPS = 0.01;
EPS2 = EPS * 2;

N_BAFFLES = nBafflesNeeded(H_BUTTRESS, "18500");

Z_BATT     = 65 + 4;
D_BATT     = 18.50 + 0.5;
D_ROD      = 3.0;   // fixme - diameter of copper rod

OLED_DISPLAY_W           = 23 + 1;
OLED_DISPLAY_L           = 32 + 1;
OLED_DISPLAY_MOUNT_W     = 17;
OLED_DISPLAY_MOUNT_L     = 26;

OLED_DX = 1;     // fixme; account for offset screen
OLED_DY = 9;

module sBattery() {
    translate([0, -R_INNER + D_BATT/2, 0]) cylinder(h=Z_BATT, d=D_BATT); 
}

module pcb() {
    DZ = 56.0;
    DY = 7.0;
    DX = 18.0;
    translate([-DX/2, -13.0, 0])
        cube(size=[DX, DY, DZ]);
}

module innerSpace() {
    translate([0, 0, M_AFT_STOP])
        cylinder(h=200, d=D_INNER);
}

// Battery section
if (DRAW_AFT) {
    difference() {
        union() {
            for(i=[0:4]) {
                dz = M_AFT_STOP + i*2*H_BUTTRESS;
                translate([0, 0, dz]) {
                    difference() {
                        intersection() {
                            T = 8;
                            translate([0, -R_INNER + D_BATT/2, 0])
                                tube(h=H_BUTTRESS, do=D_BATT+T, di=D_BATT);
                            cylinder(h=10, d=D_INNER);
                        }
                        // Channel
                        translate([-3, 0, 0])
                            cube(size=[6, 100, 100]);
                    }
                    translate([9, -7, 3])
                        baffleHalfBridge(H_BUTTRESS, 3);        
                    mirror([-1, 0, 0]) translate([9, -7, 3])
                        baffleHalfBridge(H_BUTTRESS, 3);                        
                }
            }

            for(i=[5:8]) {
                translate([0, 0, M_AFT_STOP + i*2*H_BUTTRESS]) {
                    oneBaffle(D_INNER, H_BUTTRESS,
                        battery=false,
                        mc=false,
                        noBottom=false,
                        cutoutHigh=false,
                        bridgeOnlyBottom=true);
                }
            }

            for(i=[9:13]) {
                translate([0, 0, M_AFT_STOP + i*2*H_BUTTRESS]) {
                    oneBaffle(D_INNER, H_BUTTRESS,
                        battery=false,
                        mc=true,
                        noBottom=true,
                        cutoutHigh=false,
                        bridgeOnlyBottom=true);
                }
            }
            intersection() {
                innerSpace();
                translate([OLED_DX, 0, 0]) union() {
                    translate([6, OLED_DY, M_AFT_STOP+2]) pcbButtress();
                    translate([-6, OLED_DY, M_AFT_STOP+2]) mirror([-1,0,0]) pcbButtress();
                    translate([6, OLED_DY, M_AFT_STOP+30]) pcbPillar();
                    translate([-6, OLED_DY, M_AFT_STOP+30]) mirror([-1,0,0]) pcbPillar();
                }
            }
        }
        // Battery
        translate([0, -R_INNER + D_BATT/2, 0])
            cylinder(h=200, d=D_BATT);

        // OLED
        *translate([-OLED_DISPLAY_W/2, OLED_DY, 0])
            cube(size=[OLED_DISPLAY_W, 100, OLED_DISPLAY_L]);

        // crystal
        C_Y = 10;
        translate([0, C_Y, M_AFT_STOP + H_BUTTRESS*9]) crystal(11, 9, 200);
        // Channel
        translate([-3, 0, M_AFT_STOP + H_BUTTRESS*9]) cube(size=[6, 100, 100]);

        // Rods
        translate([10, 7, M_AFT_STOP + H_BUTTRESS*12]) cylinder(h=100, d=D_ROD);
        mirror([-1,0,0]) translate([10, 7, M_AFT_STOP + H_BUTTRESS*12]) cylinder(h=100, d=D_ROD);
    }
}

if(DRAW_FORE) {
    M_START = M_AFT_STOP + H_BUTTRESS * 27;
    M_END = M_INSET_END + 4;

    translate([0, 0, M_START]) {
        insetHolder(
            D_INNER,
            D_OUTER,
            DX_INSET,
            M_END - M_START,
            H_BUTTRESS,
            M_INSET_START - M_START,
            M_INSET_END - M_START,
            M_BOLT - M_START,
            M_PORT_CENTER - M_START,
            M_SWITCH_CENTER - M_START,
            M_USB - M_START,
            roundRect = 3.175/2
        );
    }

    translate([0, 0, M_EMITTER_BASE]) {
        emitterBase(D_INNER);
    }
}

if (DRAW_EMITTER) {
    translate([0, 0, M_EMITTER_BASE]) {
        emitterHolder(D_INNER);
    }
}

*color("red") translate([0, 0, M_SWITCH_CENTER]) rotate([-90, 0, 0]) cylinder(d=16, h=12);
*color("red") translate([0, 0, M_PORT_CENTER]) rotate([-90, 0, 0]) cylinder(d=11, h=12);
*color("red") translate([-5, 0, M_USB - 1]) cube(size=[10, 12, 2]);
*color("red") translate([0, 0, M_AFT_STOP]) sBattery(); 
*color("red") translate([0, 0, M_AFT_STOP + Z_BATT]) pcb(); 

