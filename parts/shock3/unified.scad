include <dim.scad>
use <../shapes.scad>
use <../commonUnified.scad>
use <../inset.scad>

DRAW_AFT = false;
DRAW_FORE = true;

$fn=60;
EPS = 0.01;
EPS2 = EPS * 2;

Z_BATT     = 65 + 4;
D_BATT     = 18.50 + 0.5;
D_M2       = 1.7;
D_ROD       = 3.4;
D_ROD_PLUS  = 3.4 + 0.4;

// These are the values for the new (generic, not adafruit) screens.
OLED_DISPLAY_W           = 20.5 + 1;
OLED_DISPLAY_L           = 33 + 1;
OLED_DISPLAY_MOUNT_W     = 0.6 * 25.4;
OLED_DISPLAY_MOUNT_L     = 27.6; //1.1 * 25.4;

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
    keyJoint(12, slot ? D_INNER + 1 : D_INNER, D_INNER-4, false, 0);
}

module metalArt()
{
    color("gold") {
        DO_TUBE = 1.25 * 25.4;
        THICK = 0.032;
        DI_TUBE = DO_TUBE - THICK * 2 * 25.4;

        difference() {
            intersection() {
                translate([0, R_INNER - DO_TUBE/2, M_METAL_ART]) {
                    tube(h=DZ_METAL_ART, do=DO_TUBE, di=DI_TUBE);
                }
                W = 24;
                translate([-W/2, Y_METAL_ART, M_METAL_ART])
                    cube(size=[W, 100, DZ_METAL_ART]);
            }    
            *translate([-6, 0, M_CRYSTAL_START]) 
                cube(size=[12, 100, DZ_CRYSTAL_SECTION]);

            W = 18.5;
            translate([-W/2, 0, WINDOW_0_START]) 
                cube(size=[W, 100, WINDOW_0_DZ]);
            translate([-W/2, 0, WINDOW_1_START]) 
                cube(size=[W, 100, WINDOW_1_DZ]);
        }
    }
}

module reverseBridge()
{
    translate([7, 0, 0])
    polygonYZ(h=1, points=[
        [8, 1],
        [8, -6],
        [2, 0]
    ]);

    mirror([-1,0, 0])
    translate([7, 0, 0])
    polygonYZ(h=1, points=[
        [8, 1],
        [8, -6],
        [2, 0]
    ]);
}

// Battery section
if (DRAW_AFT) {
    difference() {
        union() {
            translate([0, 0, M_SPEAKER_BACK])
                rotate([0, 0, 180])
                    speakerHolder(D_AFT, M_AFT_STOP - M_SPEAKER_BACK, 3, "bass22");

            difference() {
                translate([0, 0, M_DISPLAY]) {
                    tube(h=4, do=D_THREAD, di=10);
                    tube(h=5.5, do=D_INNER, di=10);
                }
                translate([-OLED_DISPLAY_W/2 + OLED_DX, OLED_DY, M_DISPLAY-EPS]) 
                    cube(size=[OLED_DISPLAY_W, 100, 100]);
                translate([-3, 0, M_AFT_STOP-EPS]) cube(size=[6, 100, 100]);

                translate([OLED_DISPLAY_MOUNT_W/2 + OLED_DX,  OLED_DY + EPS, M_DISPLAY + POST_DY0])
                    rotate([90, 0, 0]) cylinder(h=10, d=D_M2);
                translate([-OLED_DISPLAY_MOUNT_W/2 + OLED_DX,  OLED_DY + EPS, M_DISPLAY + POST_DY0])
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
                        translate([-3, 0, -EPS])
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
                    reverseBridge();
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
                    reverseBridge();
                }
            }
            intersection() {
                innerSpace();
                translate([OLED_DX, 0, 0]) union() {
                    translate([OLED_DISPLAY_MOUNT_W/2 + OLED_DX,  OLED_DY, M_DISPLAY + POST_DY1]) 
                        pcbPillar(dBoost=2.0);
                    translate([-OLED_DISPLAY_MOUNT_W/2 + OLED_DX, OLED_DY, M_DISPLAY + POST_DY1]) 
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
        // Top metal art
        translate([-12, Y_METAL_ART+1, M_METAL_ART]) {
            cube(size=[24, 100, DZ_METAL_ART]);
        }
        metalArt();

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
        translate([0, Y_CRYSTAL, M_AFT_STOP + H_BUTTRESS*9]) 
            crystal(W_CRYSTAL, H_CRYSTAL, 200);

        // Channels
        LOWER_W = 6;
        UPPER_W = 10;
        translate([-LOWER_W/2, -EPS, M_AFT_STOP + H_BUTTRESS*9]) 
            cube(size=[LOWER_W, 100, 100]);
        translate([-UPPER_W/2, Y_CRYSTAL, M_AFT_STOP + H_BUTTRESS*9]) 
            cube(size=[UPPER_W, 100, 100]);

        // Rod
        ROD_X = 10.5;
        ROD_Y = 6.0;
        // Long one, structural support.
        translate([ROD_X, ROD_Y, M_AFT_STOP]) 
            cylinder(h=500, d=D_ROD);
        translate([ROD_X, ROD_Y, M_AFT_STOP + 10.0]) 
            cylinder(h=500, d=D_ROD_PLUS);
        
        // Short one, hold crystal.
        translate([-ROD_X, ROD_Y, M_AFT_STOP + H_BUTTRESS*10-EPS]) 
            cylinder(h=H_BUTTRESS*2, d=D_ROD);
        translate([-ROD_X, ROD_Y, M_AFT_STOP + H_BUTTRESS*12-EPS]) 
            cylinder(h=500, d=D_ROD_PLUS);
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
                    M_SWITCH_CENTER - M_START,
                    M_USB - M_START,

                    roundRect = 3.175/2,
                    firstButtressFullRing = false,
                    dyInset=3.7,
                    dySwitch=10.5
                );
            }
            intersection() {
                innerSpace();
                union() {
                    BASE_Y = R_INNER - 3.7;

                    difference() {
                        translate([-100, -R_INNER, M_START]) {
                            cube(size=[200, 3.5, H_BUTTRESS*2]);
                        }
                        cylinder(h=500, d=D_INNER-4);
                    }

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
    translate([0, 0, M_EMITTER_BASE]) tube(h=10.0, di=dynamicHeatSinkThread(), do=D_INNER);
}

*color("red") translate([0, 0, M_AFT_STOP]) sBattery(); 
*color("olive") translate([0, 0, M_AFT_STOP + Z_BATT]) pcb(); 
*color("orange") translate([0, Y_CRYSTAL, M_CRYSTAL_START]) 
    crystal(W_CRYSTAL, H_CRYSTAL, DZ_CRYSTAL_SECTION);

*color("olive") union() {
    translate([-OLED_DISPLAY_W/2 + OLED_DX, OLED_DY, M_DISPLAY-EPS]) 
        cube(size=[OLED_DISPLAY_W, 2, OLED_DISPLAY_L]);
}

*metalArt();
