include <dim.scad>
use <buttress.scad>
use <../shapes.scad>
use <vents.scad>

$fn=90;
EPS = 0.01;
EPS2 = EPS * 2;

DISPLAY_INNER_W = (DISPLAY_W - DISPLAY_MOUNT_W)/2;
DISPLAY_INNER_L = (DISPLAY_L - DISPLAY_MOUNT_L)/2;
POWER_Z         = 10;

M_SPEAKER_FRONT 	= M_SPEAKER_BACK + H_SPEAKER_HOLDER;
M_BATTERY_BACK 		= M_SPEAKER_FRONT;
M_AFT_STOP_FRONT	= M_AFT_STOP + H_AFT_RING;
M_DISPLAY_BUTTRESS 	= M_DISPLAY + DISPLAY_L - DISPLAY_INNER_L - H_BUTTRESS / 2;
M_PORT_BUTTRESS     = M_PORT_CENTER + POWER_Z / 2;
M_CRYSTAL_BUTTRESS  = M_PORT_BUTTRESS + H_BUTTRESS + H_CRYSTAL_HOLDER;

R_DISPLAY_THREAD = 0.8; // M2 bolt
R_DISPLAY_THREAD_HEAD = 2.0;
DEPTH_DISPLAY_THREAD = 4;

module battery() {
    translate([0, -BATTERY_DROP, M_BATTERY_BACK - EPS]) {
        cylinder(d=D_BATTERY, h = H_BATTERY + EPS2);
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
   	LEN     = M_DISPLAY_BUTTRESS - M_AFT_STOP_FRONT + 0.1 + INSET;
   	LENLONG = M_DISPLAY_BUTTRESS - M_AFT_STOP_FRONT + H_BUTTRESS + INSET;

    intersection() {
		innerTube();
		union() {			
		    translate([0, 0, M_AFT_STOP_FRONT - INSET]) {
		        intersection() {
		            translate([-20, -20, 0]) cube(size=[30, 17, LENLONG]);
		            tube(LENLONG, D_INNER/2 - 0.5, D_INNER/2);
		        }
			    color("yellow") {
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

module aftPowerHoles()
{
	translate([D_AFT/2 - 1.8, 0, M_WAY_BACK]) cylinder(h=H_FAR, d=2.5);
	translate([-D_AFT/2 + 1.8, 0, M_WAY_BACK]) cylinder(h=H_FAR, d=2.5);
}

 module batteryHolder()
 {
 	H = M_AFT_STOP - M_BATTERY_BACK;

    translate([0, 0, M_BATTERY_BACK]) {
	    difference() {
        	cylinder(h=H, d=D_AFT);

	    	translate([0, 0, H - 4]) {
	        	rotate([180, 0, 0]) vent1(4, H-8, 6, 20);
        	}
        }
    } 	
 }

module speakerHolder()
{
    DELTA = 0.8;
    translate([0, 0, M_SPEAKER_BACK]) {
        difference() 
        {
            cylinder(h=H_SPEAKER_HOLDER, d=D_AFT);
            translate([-X_SPEAKER/2, -Y_SPEAKER/2, -EPS]) {
                cube(size=[X_SPEAKER, Y_SPEAKER, H_SPEAKER + EPS]);
            }
            translate([-20, -Y_SPEAKER_INNER/2, -EPS]) {
                cube(size=[40, Y_SPEAKER_INNER, H_SPEAKER + EPS]);
            }
            // Sound trough.
            translate([-2, -9, -EPS]) {
                cube(size=[4, 18, H_SPEAKER + 1]);
            }
        }
    }     
}

module speakerBolts()
{
    translate([0, 0, M_SPEAKER_BACK]) {
        translate([4.5, 9.5, -EPS]) {
            cylinder(h=10, r=R_DISPLAY_THREAD);
            cylinder(h=2, r=R_DISPLAY_THREAD_HEAD);
        }
        translate([-4.5, 9.5, -EPS]) {
            cylinder(h=10, r=R_DISPLAY_THREAD);
            cylinder(h=2, r=R_DISPLAY_THREAD_HEAD);
        }
    }
}


module portButtress()
{
    POWER_X = 11;
    POWER_Y = 14.5;

    T = 2;
    OFFSET_Y = -0.4;
    OFFSET_X = -1;
    
    INNER_X0 = -POWER_X / 2 + OFFSET_X;
    INNER_Y0 = OFFSET_Y;
    INNER_Z0 = -POWER_Z / 2; // from port center.

    LED_X = 13;
    LED_Y = 8;
    LED_Z = 1.5;    
    
    translate([0, 0, M_PORT_CENTER]) {
        difference() {
            translate([INNER_X0, INNER_Y0 - T, INNER_Z0 - T]) {
                cube(size=[POWER_X + T, POWER_Y + T, POWER_Z + T]);
            }
            translate([INNER_X0-EPS, INNER_Y0, INNER_Z0]) {
                cube(size=[POWER_X+EPS, POWER_Y + EPS, POWER_Z + 10]);
            }
        }          
        
        translate([0, 0, POWER_Z/2]) {
            difference() {
                buttress(pcb=7);
                translate([-LED_X/2, CRYSTAL_Y - LED_Y/2, H_BUTTRESS - LED_Z]) {
                    cube(size=[LED_X, LED_Y, LED_Z + EPS]);
                }
            }
        }
    }
}

module crystalButtress()
{
    translate([0, 0, M_CRYSTAL_BUTTRESS]) {
        buttress(pcb=11, crystal="body", crystalHolder=9);
    }
}

//---------------------------//

// Speaker holder.
difference() {
    speakerHolder();
    speakerBolts();
    aftPowerHoles();
}

// Aft battery holder.
difference() {
	batteryHolder();
   	battery();
	mainRod();
    speakerBolts();
	aftPowerHoles();
}

// Aft lock
difference() {
	aftLock();
	display();
	mainRod();
	displayConnectors();
	aftPowerHoles();
}

// Display holder.
union() {
	difference() {
		union() {
			translate([0, 0, M_DISPLAY_BUTTRESS]) buttress(rods=true);
		}
		display();
		translate([0, 0, M_DISPLAY_BUTTRESS - EPS]) {
			D = 17;
			Y = 5;
			cylinder(d=D, h=10);
			translate([-D/2, -20, 0]) cube(size=[D, 20, 10]);
		}
	}
	displayConnectors();
}

portButtress();
crystalButtress();

