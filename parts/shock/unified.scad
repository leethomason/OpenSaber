include <dim.scad>
use <buttress.scad>
use <../shapes.scad>
use <vents.scad>

$fn=90;
EPS = 0.01;
EPS2 = EPS * 2;

DISPLAY_INNER_W = (DISPLAY_W - DISPLAY_MOUNT_W)/2;
DISPLAY_INNER_L = (DISPLAY_L - DISPLAY_MOUNT_L)/2;

M_SPEAKER_FRONT 	= M_SPEAKER_BACK + H_SPEAKER_HOLDER;
M_BATTERY_BACK 		= M_SPEAKER_FRONT;
M_AFT_STOP_FRONT	= M_AFT_STOP + H_AFT_RING + H_AFT_LIP;
M_DISPLAY_BUTTRESS 	= M_DISPLAY + DISPLAY_L - DISPLAY_INNER_L - H_BUTTRESS / 2;
M_CRYSTAL_BUTTRESS  = M_PORT_BUTTRESS + H_BUTTRESS + H_CRYSTAL_HOLDER;
M_MID_BUTTRESS      = M_CRYSTAL_BUTTRESS + H_BUTTRESS + H_CRYSTAL_HOLDER;

R_DISPLAY_THREAD = 0.8; // M2 bolt
R_DISPLAY_THREAD_HEAD = 2.0;
DEPTH_DISPLAY_THREAD = 4;

DOTSTAR_HOLDER_TRIM = 2;
DOTSTAR_HOLDER_T    = 1.5;    
DOTSTAR_START       = 3;
DOTSTAR_SPACE       = 7;

module battery() {
    translate([0, -BATTERY_DROP, M_BATTERY_BACK - EPS]) {
        cylinder(d=D_BATTERY, h = H_BATTERY + EPS2);
    }
}

module mainRod() {
	translate([X_ROD, Y_ROD, H_AFT_RING - 16]) {
		cylinder(d=D_ROD, h=H_FAR);
        translate([0, 0, -10]) {
            cylinder(d2=D_ROD, d1=0, h=10);
        }
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

module displayBolts()
{
    translate([DISPLAY_INNER_W, 0, DISPLAY_INNER_L])                            displayBolt();
    translate([DISPLAY_W - DISPLAY_INNER_W, 0, DISPLAY_INNER_L])                displayBolt();
    translate([DISPLAY_W - DISPLAY_INNER_W, 0, DISPLAY_L - DISPLAY_INNER_L])    displayBolt();
    translate([DISPLAY_INNER_W, 0, DISPLAY_L - DISPLAY_INNER_L])                displayBolt();
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
    EXTRA = 1;

    translate([-DISPLAY_W/2, Y_DISPLAY, M_DISPLAY]) {

	    translate([-EXTRA/2, 0, 0]) {
	        cube(size=[DISPLAY_W + EXTRA, 10, DISPLAY_L+1.0]);
	    }
	    displayBolts();
	    
	    translate([0, -1, 6]) {
	        cube(size=[DISPLAY_W, 1, DISPLAY_L - 12]);
	    }
	}
    translate([0, Y_DISPLAY, M_DISPLAY]) {
        X_CABLE = 12;
        Y_CABLE = 2;
        Z_CABLE = 3;
        translate([-X_CABLE/2, -Y_CABLE, -Z_CABLE]) {
            cube(size=[X_CABLE, 10, Z_CABLE + 7]);
        }
    }
}


module displayButtress()
{
    difference() {
        translate([0, 0, M_DISPLAY_BUTTRESS]) buttress(rods=true);
		display();
		translate([0, 0, M_DISPLAY_BUTTRESS - EPS]) {
			D0 = 15; // less that 18
            D1 = 19;
			Y = 5;
			cylinder(d=D0, h=10);
			translate([-D1/2, -20, 0]) cube(size=[D1, 20, 10]);
		}
		translate([12, -0.5, M_DISPLAY_BUTTRESS - EPS]) {
            cylinder(d=3, h=10);
        }
		translate([-12, -0.5, M_DISPLAY_BUTTRESS - EPS]) {
            cylinder(d=3, h=10);
        }
	}
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

module aftPowerHoles()
{
	translate([D_AFT/2 - 1.8, 0, M_WAY_BACK]) cylinder(h=H_FAR, d=2.5);
	translate([-D_AFT/2 + 1.8, 0, M_WAY_BACK]) cylinder(h=H_FAR, d=2.5);
}

 module batteryHolder()
 {
 	H = M_AFT_STOP - M_BATTERY_BACK;
    echo("H battery", H);

    translate([0, 0, M_BATTERY_BACK]) {
	    difference() {
        	cylinder(h=H, d=D_AFT);

	    	translate([0, 0, H - 4]) {
	        	rotate([180, 0, 0]) vent1(3, H-8, 6, 20);
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
                cube(size=[4, 18, 20]);	//H_SPEAKER + 1]);
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
	POWER_Z = 10;

    T = 2;
    OFFSET_Y = 0.2;
    OFFSET_X = -1;
    
    INNER_X0 = -POWER_X / 2 + OFFSET_X;
    INNER_Y0 = OFFSET_Y;
    INNER_Z0 = -POWER_Z / 2; // from port center.

    LED_X = 13;
    LED_Y = 10;
    LED_Z = 1.5; 

    Z_WITH_FILL = M_PORT_BUTTRESS - (M_PORT_CENTER - POWER_Z/2) + T;
    
    difference() {
        union() {
            translate([0, 0, M_PORT_CENTER]) {
                translate([INNER_X0, INNER_Y0 - T, INNER_Z0 - T]) {
                    cube(size=[POWER_X + T, POWER_Y + T, Z_WITH_FILL]);
                }
            }
            translate([0, 0, M_PORT_BUTTRESS]) {
                difference() {
                    buttress(pcb=7, upperWiring=true);
                    translate([-LED_X/2, CRYSTAL_Y - LED_Y/2, H_BUTTRESS - LED_Z]) {
                        cube(size=[LED_X, LED_Y, LED_Z + EPS]);
                    }
                }
            }
        }
        translate([INNER_X0-EPS, INNER_Y0, INNER_Z0 + M_PORT_CENTER]) {
            cube(size=[POWER_X+EPS, 30, POWER_Z]);
            translate([-10, 0, 0]) cube(size=[10 + POWER_X+EPS, 5, POWER_Z]);
        }   
        
    }          
}

module crystalButtress()
{
    translate([0, 0, M_CRYSTAL_BUTTRESS]) {
        buttress(pcb=12, crystal="body", crystalHolder=9, upperWiring=true, altRod=true);
    }
}

module midButtress() 
{
    translate([0, 0, M_MID_BUTTRESS]) {
        buttress(pcb=11, crystal="body", upperWiring=true);
    }
}    

module frontButtress() 

{
    translate([0, 0, M_FRONT_BUTTRESS]) {
        buttress(pcb=11, crystal="body", altRod=true, upperWiring=true);
    }
}    

module rail(angle) 
{
    RAIL_EPS    = 0.2;
    CAP         = 2;
    
    FRONT = M_FRONT_BUTTRESS + H_BUTTRESS + CAP;
    BACK = M_DISPLAY_BUTTRESS - CAP;
    LEN = FRONT - BACK;
    STOPS = [   M_DISPLAY_BUTTRESS, 
                M_PORT_BUTTRESS,
                M_CRYSTAL_BUTTRESS,
                M_MID_BUTTRESS,
                M_FRONT_BUTTRESS ];
    SIDE = (RAIL_WIDTH - NOTCH_WIDTH)/2 + RAIL_EPS;
    
    color("green") {
        rotate([0, 0, angle]) {
            difference() {
                translate([-R_INNER + RAIL_EPS, -RAIL_WIDTH/2, BACK]) {
                    cube(size=[NOTCH_DEPTH - RAIL_EPS, RAIL_WIDTH, LEN]);
                }
                for(stop = STOPS) {
                    translate([-R_INNER, -RAIL_WIDTH/2, stop - RAIL_EPS/2]) {
                        cube(size=[NOTCH_DEPTH, SIDE, H_BUTTRESS + RAIL_EPS]);
                    }
                    translate([-R_INNER, RAIL_WIDTH/2-SIDE, stop]) {
                        cube(size=[NOTCH_DEPTH, SIDE, H_BUTTRESS + RAIL_EPS]);
                    }
                }
            }
            translate([-R_INNER + RAIL_EPS, -RAIL_WIDTH/2-1, FRONT - CAP]) {
            	cube(size=[NOTCH_DEPTH - RAIL_EPS, RAIL_WIDTH+2, CAP]);
            }
        }
    }
}


module dotstars(y, pad)
{
    /*translate([-X_DOTSTAR/2 - pad/2, Y_DISPLAY + DOTSTAR_HOLDER_T, M_DISPLAY - pad/2]) {
        for(i=[0:3]) {
            translate([0, 0, DOTSTAR_START + DOTSTAR_SPACE * i]) {
                cube(size=[X_DOTSTAR + pad, y, X_DOTSTAR + pad]);
            }
        }
    }*/
    translate([0, 0, M_DISPLAY]) {
        for(i=[0:3]) {
            translate([ -X_DOTSTAR/2 - pad/2, 
                        -X_DOTSTAR/2, 
                        DOTSTAR_START + DOTSTAR_SPACE * i - pad/2]) {
                cube(size=[X_DOTSTAR + pad, y, X_DOTSTAR + pad]);
                //rotate([-90, 0, 0]) cylinder(h=30, d=D_UI);
            }
        }
    }
}

module dotstarHolder() {
    intersection()
    {
        innerTube();

        union() {
            translate([-DISPLAY_W/2, Y_DISPLAY, M_DISPLAY]) {
                difference() {
                    translate([0, 0, DOTSTAR_HOLDER_TRIM/2]) {
                        cube(size=[DISPLAY_W, DOTSTAR_HOLDER_T, DISPLAY_L - DOTSTAR_HOLDER_TRIM]);
                    }
                    displayBolts();
                }        
            }
            //color("red") dotstars(Y_DOTSTAR);
        }
    } 
}

module dotstarCap()
{
    intersection()
    {
        innerTube();

        difference() {
            translate([-DISPLAY_W/2, Y_DISPLAY + DOTSTAR_HOLDER_T + 1.0, M_DISPLAY]) {
                difference() {
                    translate([0, 0, DOTSTAR_HOLDER_TRIM/2]) {
                        cube(size=[DISPLAY_W, 10, DISPLAY_L - DOTSTAR_HOLDER_TRIM]);
                    }
                    displayBolts();
                }
            }
            translate([-6, Y_DISPLAY + DOTSTAR_HOLDER_T + 1.0, M_DISPLAY + DISPLAY_L - 6]) {
                    cube(size=[12, 10, 5]);
            }
            dotstars(20, 0.4);            
        }
    } 
    // Dotstars
    color("red") {
        translate([0, 0, M_DISPLAY]) {
            for(i=[0:3]) {
                translate([0, 10, DOTSTAR_START + DOTSTAR_SPACE * i + X_DOTSTAR/2]) {
                    //cube(size=[X_DOTSTAR, y, X_DOTSTAR]);
                    rotate([-90, 0, 0]) cylinder(h=10, d=2);
                }
            }
        }
    }
}

module drillGuide(alignment)
{
    D_UI = 2;
    D_PORT = alignment ? 2 : 8;
    D_CRYSTAL = alignment ? 2 : 12;
            
    difference() {
        W = 16;
        translate([-W/2, 2, 0]) {
            cube(size=[W, 20, 105]);
        }
        cylinder(h=200, d=D_OUTER);

        // Dotstars
        translate([0, 0, M_DISPLAY]) {
            for(i=[0:3]) {
                translate([0, 0, DOTSTAR_START + DOTSTAR_SPACE * i + X_DOTSTAR/2]) {
                    //cube(size=[X_DOTSTAR, y, X_DOTSTAR]);
                    rotate([-90, 0, 0]) cylinder(h=30, d=D_UI);
                }
            }
        }
        translate([0, 0, M_PORT_CENTER]) {
            rotate([-90, 0, 0]) cylinder(h=30, d=D_PORT);
        }
        translate([0, 0, M_CRYSTAL_VIEW_CENTER]) {
            rotate([-90, 0, 0]) cylinder(h=30, d=D_CRYSTAL);
        }
        
        // COOLING        
        translate([0, 0, 10]) rotate([-90, 0, 0]) roundedRect(h=30, d=10);
        translate([0, 0, 47]) rotate([-90, 0, 0]) roundedRect(h=30, d=6);
        translate([0, 0, 70]) rotate([-90, 0, 0]) roundedRect(h=30, d=12);
        translate([0, 0, 82]) rotate([-90, 0, 0]) roundedRect(h=30, d=6);
    }
    //color("red") dotstars(10);
}

//---------------------------//

// Speaker holder.
*difference() {
    speakerHolder();
    speakerBolts();
    aftPowerHoles();
}

// Aft battery holder.
*difference() {
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
	displayButtress();
	displayConnectors();
}

*dotstarHolder();
dotstarCap();

*portButtress();
*crystalButtress();
*midButtress();
*frontButtress();

*rail(NOTCH_ANGLE_0);
*rail(NOTCH_ANGLE_1);
*drillGuide(alignment=true);
