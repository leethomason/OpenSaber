include <dim.scad>
use <vents.scad>
use <../shapes.scad>
use <buttress.scad>
use <buttressC.scad>

EPS = 0.1;
EPS2 = EPS * 2;
$fn = 90;

H_AFT = 30;
SPACE = DISPLAY_MOUNT_L;
DISPLAY_Y = 10;

// FIXME: display heights

module display()
{
    DW = (DISPLAY_W - DISPLAY_MOUNT_W)/2;
    DL = (DISPLAY_L - DISPLAY_MOUNT_L)/2;

    difference() {
        cube(size=[DISPLAY_W, 10, DISPLAY_L]);
        translate([DW, 0, DL]) {
            rotate([-90, 0, 0]) cylinder(d=D_DISPLAY_MOUNT, h=20);
        }
        translate([DISPLAY_W - DW, 0, DL]) {
            rotate([-90, 0, 0]) cylinder(d=D_DISPLAY_MOUNT, h=20);
        }
        translate([DISPLAY_W - DW, 0, DISPLAY_L - DL]) {
            rotate([-90, 0, 0]) cylinder(d=D_DISPLAY_MOUNT, h=20);
        }
        translate([DW, 0, DISPLAY_L - DL]) {
            rotate([-90, 0, 0]) cylinder(d=D_DISPLAY_MOUNT, h=20);
        }
    }
    translate([0, -1, 6]) {
        cube(size=[DISPLAY_W, 1, DISPLAY_L - 12]);
    }
}

module aft()
{
    difference() {
        tube(H_AFT, D_BATTERY/2, D_AFT/2);    
        translate([0, 0, 2]) {
            vent4(H_AFT - H_BUTTRESS - 4, R_AFT, 20);
        }
    }
    translate([0, 0, H_AFT - H_BUTTRESS]) {
        cylinder(h=H_BUTTRESS, d=D_AFT);
    }
}


intersection()
{
    cylinder(h=100, d=D_INNER);
    difference() {
        union() {
            part(pcb=6, showBolt=true);
            translate([0, 0, SPACE]) {
                part(rods=false, battery=true);
            }
            translate([0, 0, SPACE + H_BUTTRESS]) {
                aft();
            }
            translate([-20, DISPLAY_Y-1.5, 0]) {
                cube(size=[40, 1.5, SPACE]);
            }
            translate([-20, -D_INNER/2, 0]) {
                cube(size=[40, 4, SPACE]);
            }
        }    
        translate([-DISPLAY_W/2, DISPLAY_Y, -H_BUTTRESS/2]) {
            display();
        }
    }
}
