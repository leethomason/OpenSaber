include <dim.scad>
use <buttress.scad>
use <../shapes.scad>

$fn=90;

DISPLAY_INNER_W = (DISPLAY_W - DISPLAY_MOUNT_W)/2;
DISPLAY_INNER_L = (DISPLAY_L - DISPLAY_MOUNT_L)/2;

M_SPEAKER_FRONT 	= M_SPEAKER_BACK + H_SPEAKER_HOLDER;
M_BATTERY_BACK 		= M_SPEAKER_FRONT;
M_AFT_STOP_FRONT	= M_AFT_STOP + H_AFT_RING;
M_DISPLAY_BUTTRESS 	= M_DISPLAY + DISPLAY_L - DISPLAY_INNER_L - H_BUTTRESS / 2;

R_DISPLAY_THREAD = 0.8; // M2 bolt
R_DISPLAY_THREAD_HEAD = 2.0;
DEPTH_DISPLAY_THREAD = 4;

module battery() {
    translate([0, -BATTERY_DROP, M_BATTERY_BACK]) {
        cylinder(d=D_BATTERY, h = H_BATTERY);
    }
}

module mainRod() {
	translate([X_ROD, Y_ROD, M_WAY_BACK]) {
		cylinder(d=D_ROD, h=H_FAR);
	}
}

module innerTube() {
	translate([0, 0, M_WAY_BACK]) cylinder(r=R_INNER, h=H_FAR);
}

module displayBolt() 
{
    // Pins are challenging to print small & accurate.
    // Trying holes for M2 bolts instead.
    translate([0, -DEPTH_DISPLAY_THREAD, 0]) {
        rotate([-90, 0, 0]) cylinder(r=R_DISPLAY_THREAD, h=20);
    }
}

module displayConnectors()
{
	INSET = 2;
   	LEN = M_DISPLAY_BUTTRESS - M_AFT_STOP_FRONT + 0.1 + INSET;;

    intersection() {
		innerTube();
	    translate([0, 0, M_AFT_STOP_FRONT - INSET]) {
		    color("yellow") {
			    union() {
			        intersection() {
			            translate([-20, -20, 0]) cube(size=[30, 17, LEN]);
			            tube(LEN, D_INNER/2 - 0.5, D_INNER/2);
			        }
		            translate([-15, -2, 0]) cylinder(d=4, h=LEN);
		            translate([ 14, -5, 0]) cylinder(d=6, h=LEN);
			    }
			}
		}
	}
}

module display()
{
    EXTRA = 0.5;

    translate([-DISPLAY_W/2, Y_DISPLAY, M_DISPLAY]) {

	    translate([-EXTRA/2, 0, 0]) {
	        cube(size=[DISPLAY_W + EXTRA, 10, DISPLAY_L+1.0]);
	    }
	    
	    translate([DISPLAY_INNER_W, 0, DISPLAY_INNER_L])                          	displayBolt();
	    translate([DISPLAY_W - DISPLAY_INNER_W, 0, DISPLAY_INNER_L]) 				displayBolt();
	    translate([DISPLAY_W - DISPLAY_INNER_W, 0, DISPLAY_L - DISPLAY_INNER_L])  	displayBolt();
	    translate([DISPLAY_INNER_W, 0, DISPLAY_L - DISPLAY_INNER_L])              	displayBolt();
	    
	    translate([0, -1, 6]) {
	        cube(size=[DISPLAY_W, 1, DISPLAY_L - 12]);
	    }
	}
}

module aftLock()
{
    color("red") {
        difference() {
            translate([0, 0, M_AFT_STOP]) cylinder(d=D_AFT_RING, h = H_AFT_RING);
            battery();
        }
    }
 }


difference() {
	aftLock();
	display();
	mainRod();
	displayConnectors();
}

difference() {
	union() {
		translate([0, 0, M_DISPLAY_BUTTRESS]) buttress(rods=true);
		displayConnectors();
	}
	display();
}

