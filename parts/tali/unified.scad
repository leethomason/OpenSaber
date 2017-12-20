include <dim.scad>
use <buttress.scad>
use <../shapes.scad>
use <vents.scad>

$fn=90;
EPS = 0.01;
EPS2 = EPS * 2;

BATTERY = false;

module battery() {
    color("yellow") translate([0, DY_BATTERY, M_BATTERY_BACK - EPS]) {
        cylinder(d=D_BATTERY, h = H_BATTERY + EPS2);
    }
}


// LED / button positive (above axis)
// thread negative (below axis)
//
module switch()
{
    D_OUTER_TOP = 13.8;
    D_INNER_TOP = 11.0;
    H_TOP       =  1.5;
    H_BODY      = 13;   // approx. connections below.

    color("yellow") {
        translate([0, 0, M_SWITCH_CENTER]) {
            rotate([-90, 0, 0]) {
                translate([0, 0, Y_SWITCH]) {
                    //cylinder(h = H_TOP, r1 = D_OUTER_TOP / 2, r2 = D_INNER_TOP / 2);
                    cylinder(h=5, d=D_OUTER_TOP+1);
                    translate([0, 0, -H_BODY]) {
                        cylinder(h = H_BODY, d=D_SWITCH);
                    }            
                }
            }
        }
    }
}

module innerTube() {
	translate([0, 0, M_WAY_BACK]) cylinder(r=R_INNER, h=H_FAR);
}


module aftLock()
{
    difference() {
        union() {
            translate([0, 0, M_AFT_STOP]) {
                color("red") {
                    cylinder(d=D_AFT_RING, h = H_AFT_RING);
                }
                translate([0, 0, H_AFT_RING]) {
                    cylinder(d=D_INNER, h = H_AFT_LIP);
                }
            }
        }
        battery();
    }
}

// USB in front
module mc(cutoutLow=false)
{
    color("blue") {
        // lower
        translate([-W_MC/2, 0, 0])
            cube(size=[W_MC, 5.5, 61.5]);
        // Under space
        SOLDER = 2.2;
        UNDER = cutoutLow ? 20 : 3;
        translate([-W_MC/2 + SOLDER, -UNDER, 0])
            cube(size=[W_MC - SOLDER*2, UNDER, 61.5]);
        // sd
        translate([-6, -2, -3.6])
            cube(size=[12, 7.5, 3.6]);
        // shield
        translate([-W_MC/2, 5.5, 14])
            cube(size=[W_MC, H_MC - 5.5, 53]);
        // usb
        translate([-6, -4, 61.5])
            cube(size=[12, 9.5, 1.5]);

    }
}

module usbSlot()
{

    translate([-DX_USB/2, 4, M_ZONE_2 + DZ_BUTTRESS + H_BUTTRESS/2 - SLOT_USB/2])
        cube(size=[DX_USB, 100, SLOT_USB]);
    translate([-20, 9, M_ZONE_2 + DZ_BUTTRESS - EPS])
        cube(size=[40, 10, 2.2]);
}

BODY_Z = M_EMITTER_BASE - M_AFT_STOP_FRONT - H_BUTTRESS;
T = 4;
DISPLAY_TROUGH = 13;

module arches()
{
    STEPS = 18;
    for(i=[0:STEPS-1]) {
        translate([0, 0, buttressZ(i)])
            if (i==5)
                buttress(trough=0, bridge=DZ_BUTTRESS, rail=true);
            else if (i==14)
                buttress(trough=DX_USB_INNER, bridge=DZ_BUTTRESS, rail=true);
            else
                buttress(bridge=DZ_BUTTRESS, rail=true);
    }
}

//---------------------------//

if (BATTERY) {
    color("white") battery();
}


module mainBody() 
{
    difference() {
        union() {
        }
    }
}

mainBody();

//mainRod();
//translate([0, FLOOR_Y, M_MC]) mc();
//translate([0, 0, -20]) switch();
//lockRail();
//crystal();
//RX = 10.2;
//color("red") translate([-RX/2, -10, M_ZONE_2+DZ_BUTTRESS]) cube(size=[RX, 1, 22]);
