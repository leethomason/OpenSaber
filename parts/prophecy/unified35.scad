use <../threads.scad>
use <../shapes.scad>
use <../vents.scad>
include <dim.scad>
use <buttress.scad>
use <beam.scad>

M_DOTSTAR_EDGE = M_DOTSTAR - X_DOTSTAR / 2;

INCHES_TO_MM = 25.4;
MM_TO_INCHES = 1 / 25.4;
$fn = 90;
EPS = 0.01;
EPS2 = EPS * 2;

H_HEAT_SINK_THREAD = 10.0;
D_HEAT_SINK_THREAD = 20.2;  // 20.4 is loose (PHA), 20.1 tight (PLA)
R_DOTSTAR          = 90 + 180 * (TANO_VERSION - 1);
SPKR_OFFSET         = 7;    // distance from back of pommel to speaker
D_SPKR_INNER = D_SPKR_PLASTIC - 4;


DOTSTAR_WALL       = Y_DOTSTAR + 0.5;

module innerTube()
{
    translate([0, 0, M_WAY_BACK]) {
        cylinder(d=D_AFT, h=M_TRANSITION - M_WAY_BACK);
    }
    translate([0, 0, M_TRANSITION]) {
        cylinder(d=D_FORWARD, h = H_FAR);
    }
}

module heatSink()
{
    translate([0, 0, M_LED_HOLDER_FRONT - H_HEAT_SINK_THREAD]) {
        cylinder(h=H_HEAT_SINK_THREAD + 0.2, d=D_HEAT_SINK_THREAD);
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
            rotate([R_DOTSTAR, 0, 0]) {
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

module dotstars(y, strip)
{
    translate([-X_DOTSTAR/2, -y, 0]) {
        for(i=[0:3]) {
            PAD = 0.4;
            translate([-PAD/2, 0, DOTSTAR_SPACE * i - PAD/2]) {
                cube(size=[X_DOTSTAR + PAD, y, X_DOTSTAR + PAD]);
            }
        }
    }   
    if (strip) {
        X_PAD = 7;  // min 7
        END_PAD = 2.5;
        X_STRIP = X_DOTSTAR + X_PAD;
        Z_STRIP = DOTSTAR_SPACE * 3 + X_DOTSTAR + END_PAD;
        T = 3;

        translate([-X_DOTSTAR/2 - X_PAD/2, 0, 0]) {
            cube(size=[X_STRIP, T, Z_STRIP]);
        }
    }
}

module dotstarsPositioned(y, strip)
{
    rotate([0, 0, R_DOTSTAR]) {
        translate([0, -R_FORWARD + DOTSTAR_WALL + EPS, M_DOTSTAR_EDGE]) {
            dotstars(y, strip);
        }
    }    
}

module ledHolder()
{
    H = M_LED_HOLDER_FRONT - M_LED_HOLDER_BACK;
    difference() {
        translate([0, 0, M_LED_HOLDER_BACK]) cylinder(h=H, d=D_FORWARD);
        heatSink();
        dotstarsPositioned(20, true);
    }
}

module speaker(pad=0, dpad=0)
{
    translate([0, 0, -pad/2]) cylinder(h=H_SPKR_PLASTIC + pad, d=D_SPKR_PLASTIC + dpad);
    translate([0, 0, H_SPKR_PLASTIC]) cylinder(h=H_SPKR_METAL, d=D_SPKR_METAL + dpad);
}

module speakerHolder()
{
    // Locking ring.
    difference() {
        translate([0, 0, M_POMMEL_FRONT]) {
            difference() {
                cylinder(h=M_SPKR_RING - M_POMMEL_FRONT, d=D_AFT_RING);
                cylinder(h=M_SPKR_RING - M_POMMEL_FRONT, d=D_SPKR_INNER);
            }
        } 
    }

    // Actual holder.
    H_POMMEL = M_POMMEL_FRONT - M_POMMEL_BACK;
    translate([0, 0, M_POMMEL_BACK]) {
        difference() {
            tube(H_POMMEL, D_SPKR_INNER/2, D_POMMEL/2);
            translate([-20, 4, 0]) cube(size=[40, 40, 40]);

            translate([0, 0, SPKR_OFFSET]) {
                speaker(0.8, 0.6);
            }
            translate([-20, -20, 12]) cube(size=[16, 15, 22]);
            mirror([-1, 0, 0]) translate([-20, -20, 12]) cube(size=[16, 15, 22]);
        }
    }    
}

module speakerRing()
{
    PAD = 1;    // space for foam, etc.

    translate([0, 0, M_POMMEL_BACK]) {
        difference() {
            cylinder(h=SPKR_OFFSET - PAD, d=D_POMMEL);
            cylinder(h=SPKR_OFFSET - PAD, d=D_SPKR_PLASTIC - 2);
        }
    }
}

module switchAndPortHolder()
{
    POWER_X         = 11;
    POWER_Y         = 14.5;
    POWER_Z         = 10;
    POWER_OFFSET_Y  = 0.2;
    POWER_OFFSET_X  = -1;

    H = (M_LED_HOLDER_BACK - M_TRANSITION + T_TRANSITION_RING);
    T = 3;
    T_ACCEL = 2;

    intersection() {
        translate([0, 0, M_WAY_BACK]) cylinder(h=H_FAR, d=D_FORWARD);

        difference() {
            union() {
                translate([-20, Y_SWITCH - T, M_TRANSITION - T_TRANSITION_RING]) {
                    cube(size=[40, T+1.5, H]);
                }
                
                // Build up port holder for standard power port.
                // (Never get to use that kind.)
                W = 2 * (M_PORT_CENTER - M_TRANSITION); // fancy; snaps into ring.
                difference() 
                {
                    translate([-W/2, Y_SWITCH, M_PORT_CENTER - W/2]) {
                        cube(size=[W, 6 - T, W]);
                    }
                    // Slope the front.
                    translate([-W/2, Y_SWITCH, M_PORT_CENTER + W/2]) {
                        rotate([-45, 0, 0]) {
                            cube(size=[W, W, W]);
                        }
                    }
                }
            }
            // switch
            translate([0, 0, M_SWITCH_CENTER]) {
                rotate([-90, 0, 0]) {
                    cylinder(h=20, d=D_SWITCH);
                    translate([0, 0, Y_SWITCH]) cylinder(h=20, d=D_SWITCH_TOP);
                }        
            }

            // Port (std)
            translate([0, 0, M_PORT_CENTER]) {
                rotate([-90, 0, 0]) {
                    cylinder(h=20, d=D_SMALL_PORT);
                }
            }
            transitionRing();
            heatSink();
        }
    }
}


module dotstarHolder() {
    intersection()
    {
        innerTube();

        union()
        {
            difference()
            {
                OFFSET = 31;
                rotate([0, 0, R_DOTSTAR]) translate([-10, -R_FORWARD - 1, M_TRANSITION + OFFSET]) {
                    cube(size=[20, DOTSTAR_WALL + 2, M_LED_HOLDER_BACK - M_TRANSITION - OFFSET]);
                }
                color("yellow") dotstarsPositioned(20, true);
                /*
                translate([0, -R_FORWARD + DOTSTAR_WALL + EPS, M_DOTSTAR_EDGE]) {
                    dotstars(10);
               }*/
            }
        }
    } 
}

module forwardRail()
{
    intersection()
    {
        translate([0, 0, M_TRANSITION - T_TRANSITION_RING]) {
            cylinder(d=D_FORWARD, h=M_LED_HOLDER_BACK - M_TRANSITION + T_TRANSITION_RING);
        }
        union() {
            W = 8;
            H = 2.5;
            SPACE = 0.4;

            // join
            translate([-W/2 + SPACE, -R_FORWARD + SPACE, M_TRANSITION - T_TRANSITION_RING]) {
               cube(size=[W - SPACE*2, H - SPACE*2, T_TRANSITION_RING + EPS]);
            }
            // main
            translate([-W/2, -R_FORWARD, M_TRANSITION]) {
                cube(size=[W, H, 100]);
            }
        }
    }
    // Add a ridge for positioning.
    intersection() {
        translate([0, 0, M_TRANSITION]) {
            cylinder(d=D_FORWARD, h=M_LED_HOLDER_BACK - M_TRANSITION);
        }
        translate([-4, -R_FORWARD, M_TRANSITION - T_TRANSITION_RING]) rotate([10, 0, 0]) {
            cube(size=[8, 5, 100]);
        }
    }
}

module transitionRing()
{
    R = [RAIL_ANGLE_0, RAIL_ANGLE_1];
    R_INNER = D_FORWARD/2;

    difference() {
        intersection() {
            cylinder(h=H_FAR, d=D_AFT);
            union() {
                translate([0, 0, M_TRANSITION - T_TRANSITION_RING]) {
                    tube(T_TRANSITION_RING, R_INNER, D_AFT/2);
                }
                difference() {
                    translate([-20, -R_FORWARD, M_TRANSITION - T_TRANSITION_RING]) {
                        cube(size=[40, 4, T_TRANSITION_RING]);
                    }            
                    translate([-4, -R_FORWARD, M_TRANSITION - T_TRANSITION_RING]) {
                        cube(size=[8, 2.5, T_TRANSITION_RING]);
                    }
                }
                translate([-20, Y_SWITCH - 3, M_TRANSITION - T_TRANSITION_RING]) {
                    cube(size=[40, 2, T_TRANSITION_RING]);
                }  
            }
        }
    }
}

module rail(r, m=0, h=0)
{
   H = h ? h :M_TRANSITION - M_RAIL_START - T_TRANSITION_RING;
   M = m ? m : M_RAIL_START + H/2;

    difference() {
        intersection()
        {
            innerTube();
            rotate([0, 0, r]) {
                translate([R_AFT - X_RAIL/2, 0, M + H/2]) {
                    rotate([0, 0, -r]) {
                        cube(size=[20, Y_RAIL, H], center=true);
                    }
                }            
            }        
        }
        W = W_WING + 1;
   	    translate([-W/2, -40, M_WAY_BACK]) {
	        cube(size=[W, 120, H_FAR]);
	    }
    }
}

module teensy35()
{
    OFFSET = 20;

    difference() {
        translate([-W_MC/2, Y_MC - OFFSET, M_BUTTRESS_0 + H_BUTTRESS]) {
            cube(size=[W_MC, H_MC + OFFSET, Z_MC_35]);
        }
        translate([-W_MC/2, Y_MC + H_MC - 2, M_BUTTRESS_0 + H_BUTTRESS + 50]) {
            cube(size=[W_MC, 50, 50]);
        }
    }
}

FLATTEN = 1.8;
M_B0_FRONT = M_BUTTRESS_0 + H_BUTTRESS;
M_B3_FRONT = M_BUTTRESS_3 + H_BUTTRESS;
M_B4_FRONT = M_BUTTRESS_4 + H_BUTTRESS;
Z_B3 = M_BUTTRESS_4 + EPS - M_B3_FRONT;
Z_B4 = M_TRANSITION + EPS - M_B4_FRONT - T_TRANSITION_RING;

// front
union() {
    ledHolder();
    switchAndPortHolder();
    dotstarHolder();
    forwardRail();
}

// Back battery holder
*difference() {
    intersection() {
        innerTube();
        union() {
            translate([0, 0, M_BUTTRESS_3]) buttress(mc=false, highHoles=false, leftWiring=false, rightWiring=false);
            translate([0, 0, M_BUTTRESS_4]) buttress(battery=false, trough=10, mc=false, leftWiring=false, rightWiring=false);

            translate([0, 3, 0 ]) {
                BW = 4;
                W_MC = 18;
                translate([W_MC/2, 0, M_B3_FRONT])          beam(4, 8, Z_B3);
                translate([-W_MC/2 - BW, 0, M_B3_FRONT])    beam(4, 8, Z_B3);
                translate([W_MC/2, 0, M_B4_FRONT])          beam(4, 8, Z_B4);
                translate([-W_MC/2 - BW, 0, M_B4_FRONT])    beam(4, 8, Z_B4);
            }                
        }
    }
    H_REM = M_BUTTRESS_4 + H_BUTTRESS + EPS - M_BUTTRESS_3;
    translate([-20, -10, M_BUTTRESS_3 - EPS]) cube(size=[ 7, 24, H_REM]);
    // Take out left side for wiring.
    translate([ 13, -10, M_BUTTRESS_3 - EPS]) cube(size=[20, 24, H_REM]);

    // Flatten the bottom for printing.
    translate([-20, -D_AFT_RING/2, M_WAY_BACK]) cube(size=[40, FLATTEN, H_FAR]);

    teensy35();

    translate([0, 0, M_B0_FRONT]) shelfBeam(M_TRANSITION - T_TRANSITION_RING + EPS - M_B0_FRONT,
    true, 4, true);
    translate([0, 0, M_B0_FRONT]) shelfBeam(M_TRANSITION - T_TRANSITION_RING + EPS - M_B0_FRONT, false, 4, true);

}

// Back body
difference() {
    
    union() {
        transitionRing();
        
        union() {
            translate([0, 0, M_B0_FRONT]) shelfBeam(M_TRANSITION - T_TRANSITION_RING + EPS - M_B0_FRONT,
            true, 4);
            translate([0, 0, M_B0_FRONT]) shelfBeam(M_TRANSITION - T_TRANSITION_RING + EPS - M_B0_FRONT, false, 4);

        }
        intersection() {
            innerTube();
            union() {
                translate([-W_MC/2, -R_AFT, M_BUTTRESS_0 + H_BUTTRESS + Z_MC_35]) {
                    cube(size=[W_MC, 7.5, 2]);
                }
                W = 2;
                translate([-W/2, -R_AFT, M_BUTTRESS_0 + H_BUTTRESS + Z_MC_35]) {
                    cube(size=[W, 7.5, EPS + M_TRANSITION - T_TRANSITION_RING - (M_BUTTRESS_0 + H_BUTTRESS + Z_MC_35)]);
                }
            }
        }
        translate([0, 0, M_BUTTRESS_0]) buttress(mc=false, trough = 8, leftWiring=false, rightWiring=false);

        speakerHolder();

    }
    // Flatten the bottom for printing.
    translate([-20, -D_AFT_RING/2, M_WAY_BACK]) cube(size=[40, FLATTEN, H_FAR]);
    
    // Take out a chunk for access to the USB port.
    X_USB = 11; // 10 to tight fit
    Y_USB = 9;
    Z_USB = 20;
    translate([-X_USB/2, -R_AFT, M_POMMEL_BACK]) cube(size=[X_USB, Y_USB, 35]);
    

    teensy35();
}

translate([0, 0, 70]) {
    //color("yellow") battery(12);
    //circuitry(10);
}

*switch();
*color("yellow") heatSink();
*color("yellow") speaker();
*color("yellow") speakerHolder();
*color("yellow") teensy35();

*buttress();
