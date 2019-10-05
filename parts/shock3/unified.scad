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

Z_BATT     = 65 + 4;
D_BATT     = 18.50 + 0.5;
D_M2       = 1.7;
D_ROD      = 3.4 + 0.4;

// These are the values for the new (generic, not adafruit) screens.
OLED_DISPLAY_W           = 20.5 + 1;
OLED_DISPLAY_L           = 33 + 1;
OLED_DISPLAY_MOUNT_W     = 0.6 * 25.4;
OLED_DISPLAY_MOUNT_L     = 1.1 * 25.4;

OLED_DX = -1.3;     // fixme; account for offset screen
OLED_DY = 9;
POST_DY0 = (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L) / 2;
POST_DY1 = OLED_DISPLAY_L - (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L) / 2;
                    

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

module thisKeyJoint(slot)
{
    keyJoint(12, D_INNER, D_INNER-4, false, 0);
}


// Battery section
if (DRAW_AFT) {
    difference() {
        union() {
            DZ_SPKR = 8;
//            translate([0, 0, M_AFT_STOP-2])
//                tube(h=2+EPS, do=D_AFT, di=D_AFT-4);
            translate([0, 0, M_SPEAKER_BACK])
                rotate([0, 0, 180])
                    speakerHolder(D_AFT, M_AFT_STOP - M_SPEAKER_BACK, 4, "bass22");

            difference() {
                // FIXME this shouldn't be a tight fit (in z) on the thread
                translate([0, 0, M_AFT_STOP]) tube(h=5, do=D_THREAD, di=10);
                translate([-OLED_DISPLAY_W/2 + OLED_DX, OLED_DY, M_AFT_STOP-EPS]) 
                    cube(size=[OLED_DISPLAY_W, 100, 100]);
                translate([-3, 0, M_AFT_STOP-EPS]) cube(size=[6, 100, 100]);

                translate([OLED_DISPLAY_MOUNT_W/2 + OLED_DX,  OLED_DY + EPS, M_AFT_STOP + POST_DY0])
                    rotate([90, 0, 0]) cylinder(h=10, d=D_M2);
                translate([-OLED_DISPLAY_MOUNT_W/2 + OLED_DX,  OLED_DY + EPS, M_AFT_STOP + POST_DY0])
                    rotate([90, 0, 0]) cylinder(h=10, d=D_M2);
            }

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
                    translate([9, -8.5, 3])
                        baffleHalfBridge(H_BUTTRESS, 3);        
                    mirror([-1, 0, 0]) translate([9, -8.5, 3])
                        baffleHalfBridge(H_BUTTRESS, 3);                        
                }
            }

            for(i=[5:9]) {
                translate([0, 0, M_AFT_STOP + i*2*H_BUTTRESS]) {
                    oneBaffle(D_INNER, H_BUTTRESS,
                        battery=false,
                        mc=false,
                        noBottom=false,
                        cutoutHigh=false,
                        bridgeOnlyBottom=true);
                }
            }

            for(i=[10:13]) {
                translate([0, 0, M_AFT_STOP + i*2*H_BUTTRESS]) {
                    oneBaffle(D_INNER, H_BUTTRESS,
                        battery=false,
                        mc=true,
                        noBottom=true,
                        cutoutHigh=false,
                        bridgeOnlyBottom=true,
                        bridge=i < 13 ? 1 : 0);
                }
            }
            intersection() {
                innerSpace();
                translate([OLED_DX, 0, 0]) union() {
                    //translate([OLED_DISPLAY_MOUNT_W/2 + OLED_DX,  OLED_DY, M_AFT_STOP + POST_DY0]) pcbButtress();
                    //translate([-OLED_DISPLAY_MOUNT_W/2 + OLED_DX, OLED_DY, M_AFT_STOP + POST_DY0]) mirror([-1,0,0]) pcbButtress();
                    translate([OLED_DISPLAY_MOUNT_W/2 + OLED_DX,  OLED_DY, M_AFT_STOP + POST_DY1]) 
                        pcbPillar(dBoost=2.0);
                    translate([-OLED_DISPLAY_MOUNT_W/2 + OLED_DX, OLED_DY, M_AFT_STOP + POST_DY1]) 
                        mirror([-1,0,0]) pcbPillar(dBoost=2.0);
                }
            }
            intersection() {
                innerSpace();
                union() {
                    translate([-12, -12, M_AFT_STOP]) 
                        cube(size=[4, 4, H_BUTTRESS * 10]);
                    mirror([-1, 0, 0]) translate([-12, -12, M_AFT_STOP]) 
                        cube(size=[4, 4, H_BUTTRESS * 10]);
                }                
            }

            translate([0, 0, H_BUTTRESS*29]) {
                thisKeyJoint(false);
            }
        }
        // Bottom channel
        translate([-5, -50, M_AFT_STOP]) {
            cube(size=[10, 50, 200]);
        }

        // Battery
        translate([0, -R_INNER + D_BATT/2, M_AFT_STOP])
            cylinder(h=Z_BATT, d=D_BATT);

        // OLED
        *translate([-OLED_DISPLAY_W/2, OLED_DY, 0])
            cube(size=[OLED_DISPLAY_W, 100, OLED_DISPLAY_L]);

        // crystal
        C_Y = 10;
        translate([0, C_Y, M_AFT_STOP + H_BUTTRESS*9]) 
            crystal(14, 9, 200);
        // Channel
        translate([-3, 0, M_AFT_STOP + H_BUTTRESS*9]) cube(size=[6, 100, 100]);

        // Rod
        ROD_X = 11;
        ROD_Y = 6.5;
        // Long one, structural support.
        translate([ROD_X, ROD_Y, M_AFT_STOP]) 
            cylinder(h=500, d=D_ROD);
        // Short one, hold crystal.
        translate([-ROD_X, ROD_Y, M_AFT_STOP + H_BUTTRESS*10]) 
            cylinder(h=500, d=D_ROD);
    }
}

if(DRAW_FORE) {
    M_START = M_AFT_STOP + H_BUTTRESS * 27;
    M_END = M_INSET_END + 4;

    difference() {
        union() {
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
                    0, // use flat port holderM_PORT_CENTER - M_START,
                    0, // use flat switch holder M_SWITCH_CENTER - M_START,
                    M_USB - M_START,

                    roundRect = 3.175/2,
                    firstButtressFullRing = false,
                    dyInset=3.7
                );
            }
            intersection() {
                innerSpace();
                union() {

                    // Switch
                    SWITCH_DZ = 5.7;
                    SWITCH_T = 3;
                    BASE_Y = R_INNER - 3.7;

                    translate([-50, BASE_Y - SWITCH_T - 2.0, M_SWITCH_CENTER - SWITCH_DZ/2])
                        cube(size=[100, SWITCH_T, SWITCH_DZ]);

                    // Port
                    PORT_T = 5.0;
                    PORT_DZ = 14;
                    D_PORT = 7.9;
                    difference() {
                        translate([-50, BASE_Y-PORT_T, M_PORT_CENTER - PORT_DZ/2])
                            cube(size=[100, PORT_T, PORT_DZ]);
                        translate([0, 0, M_PORT_CENTER])
                            rotate([-90, 0, 0])
                                cylinder(h=100, d=D_PORT);
                    }
                }
            }
        }
        translate([0, 0, H_BUTTRESS*29]) {
            thisKeyJoint(true);
        }
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

