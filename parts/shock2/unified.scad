include <dim.scad>
use <buttress.scad>
use <../shapes.scad>
use <vents.scad>
use <emitter.scad>
use <swoop.scad>

$fn=90;
EPS = 0.01;
EPS2 = EPS * 2;

DISPLAY_INNER_W = (DISPLAY_W - DISPLAY_MOUNT_W)/2;
DISPLAY_INNER_L = (DISPLAY_L - DISPLAY_MOUNT_L)/2;

M_SPEAKER_FRONT 	= M_SPEAKER_BACK + H_SPEAKER_HOLDER;
M_BATTERY_BACK 		= M_SPEAKER_FRONT;
M_AFT_STOP_FRONT	= M_AFT_STOP + H_AFT_RING + H_AFT_LIP;
M_DISPLAY_BUTTRESS 	= M_DISPLAY + DISPLAY_L - DISPLAY_INNER_L - H_BUTTRESS / 2;

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
                    cylinder(h = H_TOP, r1 = D_OUTER_TOP / 2, r2 = D_INNER_TOP / 2);
                    translate([0, 0, -H_BODY]) {
                        cylinder(h = H_BODY, d=D_SWITCH);
                    }            
                }
            }
        }
    }
}

module mainRod() {
	color("yellow") {
        translate([X_ROD, Y_ROD, M_WAY_BACK]) {
    		cylinder(d=D_ROD, h=H_FAR);
        }
        translate([-X_ROD, Y_ROD, M_WAY_BACK]) {
            cylinder(d=D_ROD, h=H_FAR);
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
        Z_CABLE = 3 + 1;    // cheat so we don't get a super thin piece in the locking ring.
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

module powerHolder(addative)
{
    T = 2;
    KNOCKOUT = 10;
    difference() {
       translate([-POWER_DX/2 + POWER_X - T, POWER_Y - T, M_PORT_CENTER - POWER_DZ/2 - T])
           cube(size=[POWER_DX + T*2, POWER_DY + T + 10, POWER_DZ + T*2]);
       if (addative) {
            translate([-POWER_DX/2 + POWER_X, POWER_Y, M_PORT_CENTER - POWER_DZ/2 - KNOCKOUT])
                cube(size=[POWER_DX, POWER_DY+10, POWER_DZ + KNOCKOUT]);
       }
       mainRod();
    }
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

MAIN_DISPLAY = true;
MAIN_CRYSTAL = false;
MAIN_EMITTER = false;

M_ZONE_0 = M_AFT_STOP_FRONT;
M_ZONE_1 = M_CRYSTAL_START + 0.5;
M_ZONE_2 = M_SWITCH_CENTER - 5;
M_ZONE_3 = M_EMITTER_BASE;


// Aft lock
if (MAIN_DISPLAY)
{
    difference() {
        aftLock();
        display();
        mainRod();
        aftPowerHoles();
        translate([-20, Y_FLATTEN-20, -20]) cube(size=[40, 20, 100]);
    }
}

if (MAIN_EMITTER) {
   difference() {
        translate([0, 0, M_EMITTER_BASE]) emitterBase();
        translate([-20, Y_FLATTEN-20, M_EMITTER_BASE - 5]) cube(size=[40, 20, 20]);
    }
}


module mainBody() {
    DY = -12;
    DZ = M_EMITTER_BASE - M_AFT_STOP_FRONT - H_BUTTRESS;
    SWITCH_DX = 14;
    SWITCH_DZ = 15;
    PORT_DZ = 8;
    T = 4;

    intersection() {
        union() {
            if (MAIN_DISPLAY) translate([0, 0, M_ZONE_0]) 
                cylinder(h=M_ZONE_1 - M_ZONE_0, d=D_INNER);
            if (MAIN_CRYSTAL) translate([0, 0, M_ZONE_1]) 
                cylinder(h=M_ZONE_2 - M_ZONE_1, d=D_INNER);
            if (MAIN_EMITTER) translate([0, 0, M_ZONE_2])
                cylinder(h=M_ZONE_3 - M_ZONE_2, d=D_INNER);
        }
        union() {
            difference() {
                union() {
                    STEPS = 16;
                    STEP_Z = DZ / (STEPS-1);
                    for(i=[0:STEPS-1]) {
                        translate([-20, DY, M_AFT_STOP_FRONT + i*STEP_Z]) 
                            xArch(18, 1.2, 40, 2);    //xArch(21, 1, 40, 2.5)
                        translate([-20, DY-20, M_AFT_STOP_FRONT + i*STEP_Z + 2])
                            cube(size=[40, 20, 6]);
                    }
                    //translate([-20, DY-20, M_AFT_STOP_FRONT]) cube(size=[40, 20, DZ]);
                }
                // Switch cut-out
                translate([-SWITCH_DX/2, DY, M_SWITCH_CENTER - SWITCH_DZ]) cube(size=[SWITCH_DX, 100, 100]);
                translate([0, 0,  M_SWITCH_CENTER - SWITCH_DZ]) cylinder(h=100, d=D_INNER_CORE);

                // Port cut-out
                translate([0, 0,  M_ZONE_0]) cylinder(h=M_ZONE_1 - M_ZONE_0 + EPS, d=D_INNER_CORE);
                //translate([-5, 0,  M_ZONE_0]) cube(size=[10, 100, M_ZONE_1 - M_ZONE_0]);
                powerHolder(false);

                // Display cut outs
                display();
                DISPLAY_TROUGH = 13;
                translate([-DISPLAY_TROUGH/2, DY, M_DISPLAY]) 
                    cube(size=[DISPLAY_TROUGH, 100, M_ZONE_1 - M_DISPLAY]);

                // Crystal cut-out
                translate([0, Y_CRYSTAL, M_CRYSTAL_START]) {
                    scale([W_CRYSTAL, H_CRYSTAL, 1]) {
                        cylinder(h=Z_CRYSTAL, d=1);
                    }                
                }
                translate([0, Y_CRYSTAL-1, M_CRYSTAL_START]) {
                    rotate([0, 0, 45]) cube(size=[20, 20, Z_CRYSTAL]);
                }

                // MC cut out
                translate([-W_MC/2, DY, M_MC]) 
                    cube(size=[W_MC, H_MC, Z_MC_35]);
                MC_TROUGH = 8;                
                translate([-MC_TROUGH/2, DY, M_MC]) 
                    cube(size=[MC_TROUGH, 100, Z_MC_35]);


                // Flatten
                translate([-20, Y_FLATTEN-20, M_AFT_STOP_FRONT]) cube(size=[40, 20, DZ]);
                // Rods
                mainRod();
            }    
            // Switch plate.
            difference() {
                PAD = 4;
                translate([-D_SWITCH/2 - PAD, Y_SWITCH-T, M_SWITCH_CENTER - D_SWITCH/2 - 3]) 
                    cube(size=[D_SWITCH + PAD * 2, T, 100]);
                switch();
            }
            // Port plate
            powerHolder(true);
            /*difference() {
                PAD = 4;
                translate([-D_PORT/2 - PAD, Y_SWITCH-T, M_PORT_CENTER - D_PORT]) 
                    cube(size=[D_PORT + PAD * 2, T, D_PORT*2]);
                translate([0, 0, M_PORT_CENTER]) rotate([-90, 0, 0]) cylinder(h=100, d=D_PORT);
            }*/
        }
    }
}

mainBody();
//mainRod();