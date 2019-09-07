include <dim.scad>
use <../shapes.scad>
use <../commonUnified.scad>
use <../inset.scad>

$fn=60;
EPS = 0.01;
EPS2 = EPS * 2;

N_BAFFLES = nBafflesNeeded(H_BUTTRESS, "18500");

Z_BATT     = 65 + 4;
D_BATT     = 18.50 + 0.5;

OLED_DISPLAY_W           = 23 + 1;
OLED_DISPLAY_L           = 32 + 1;
OLED_DISPLAY_MOUNT_W     = 17;
OLED_DISPLAY_MOUNT_L     = 26;

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
difference() {
    union() {
        for(i=[6:11]) {
            translate([0, 0, M_AFT_STOP + i*2*H_BUTTRESS]) {
                oneBaffle(D_INNER, H_BUTTRESS,
                    battery=false,
                    mc=false,
                    noBottom=false,
                    cutoutHigh=false);
            }
        }
        for(i=[0:5]) {
            dz = M_AFT_STOP + i*2*H_BUTTRESS;
            translate([0, 0, dz]) {
                intersection() {
                    T = 8;
                    translate([0, -R_INNER + D_BATT/2, 0])
                        tube(h=H_BUTTRESS, do=D_BATT+T, di=D_BATT);
                    cylinder(h=10, d=D_INNER);
                }
                translate([9, -7, 3])
                    baffleHalfBridge(H_BUTTRESS, 3);        
                mirror([-1, 0, 0]) translate([9, -7, 3])
                    baffleHalfBridge(H_BUTTRESS, 3);        
            }
        }
        intersection() {
            innerSpace();
            union() {
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

    // Channel
    translate([-4, 0, 0])
        cube(size=[8, 100, 100]);

    // crystal
    C_Y = 10;
    translate([0, C_Y, H_BUTTRESS*13]) crystal(11, 9, 200);
}
/*

translate([0, 0, M_DISPLAY])
    oledHolder(D_INNER, 4, DZ_DISPLAY, 0, 10);

translate([0, 0, M_DISPLAY_FRONT]) {
    baffleMCBattery(D_INNER, N_BAFFLES, H_BUTTRESS, nPostBaffles=1);
}

M_BATTERY_FRONT = M_DISPLAY_FRONT + zLenOfBaffles(N_BAFFLES + 1, H_BUTTRESS);

translate([0, 10, M_CRYSTAL_VIEW_START])
    crystal(dz=DZ_CRYSTAL_VIEW);

translate([0, 0, M_INSET_START]) {
    DZ = M_INSET_END - M_INSET_START;
    insetHolder(D_INNER, D_OUTER, DZ,
                DZ/2, 
                M_PORT_CENTER - M_INSET_START, 
                M_SWITCH_CENTER - M_INSET_START,
                17.0,
                H_BUTTRESS);
}
*/

color("red") translate([0, 0, M_SWITCH_CENTER]) rotate([-90, 0, 0]) cylinder(d=16, h=12);
color("red") translate([0, 0, M_PORT_CENTER]) rotate([-90, 0, 0]) cylinder(d=11, h=12);
color("red") translate([-5, 0, M_USB - 1]) cube(size=[10, 12, 2]);
*color("red") translate([0, 0, M_AFT_STOP]) sBattery(); 
color("red") translate([0, 0, M_AFT_STOP + Z_BATT]) pcb(); 

