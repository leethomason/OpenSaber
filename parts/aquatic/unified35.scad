
use <../threads.scad>
use <../shapes.scad>
use <../vents.scad>
include <dim.scad>
use <buttress.scad>
use <beam.scad>

DRAW_FRONT = false;
DRAW_BACK  = false;
DRAW_BAT   = true;

INCHES_TO_MM = 25.4;
MM_TO_INCHES = 1 / 25.4;
$fn = 90;
EPS = 0.01;
EPS2 = EPS * 2;

H_HEAT_SINK_THREAD = 10.0;
D_HEAT_SINK_THREAD = 20.2;  // 20.4 is loose (PHA), 20.1 tight (PLA)

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
            translate([0, 0, Y_SWITCH]) {
                cylinder(h = H_TOP, r1 = D_OUTER_TOP / 2, r2 = D_INNER_TOP / 2);
                translate([0, 0, -H_BODY]) {
                    cylinder(h = H_BODY, d=D_SWITCH);
                }            
            }
        }
    }
}

module ledHolder()
{
    H = M_LED_HOLDER_FRONT - M_LED_HOLDER_BACK;
    difference() {
        translate([0, 0, M_LED_HOLDER_BACK]) cylinder(h=H, d=D_FORWARD);
        heatSink();
    }
}

module speaker(pad=0, dpad=0)
{
    translate([0, 0, -pad/2]) cylinder(h=H_SPKR_PLASTIC + pad, d=D_SPKR_PLASTIC + dpad);
}

module speakerHolder()
{
    // Locking ring.
    difference() {
        difference() {
            translate([0, 0, M_POMMEL_FRONT]) {
                cylinder(h=M_SPKR_RING - M_POMMEL_FRONT, d=D_AFT_RING);
            }
            translate([0, 0, M_POMMEL_FRONT]) {
                cylinder(h=M_SPKR_RING - M_POMMEL_FRONT, d=D_SPKR_INNER);
            }
            translate([0, 0, M_POMMEL_BACK + SPKR_OFFSET]) {
                speaker(0, 0);
            }
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
                W =  D_PORT;
                DZ0 = 3;

                hull() {
                    OFFSET = 2;
                    translate([-W/2, Y_SWITCH, M_PORT_CENTER - W/2 - DZ0]) {
                        cube(size=[W, 6 - T, W + DZ0]);
                    }
                    translate([-W/2, Y_SWITCH, M_PORT_CENTER - W/2 - OFFSET - DZ0]) {
                        cube(size=[W, EPS, W + OFFSET*2 + DZ0]);
                    }
                }

                // Holders for the PCB
                difference() {
                    union() {
                        NOTCH = 1;
                        // First one is also for positioning
                        translate([0, -R_FORWARD + 2, M_TRANSITION-2]) triBump(3, 2, 4);
                        translate([0, -R_FORWARD + 2.5, M_TRANSITION+2]) triBump(3, NOTCH, NOTCH*2);
                        translate([0,  Y_SWITCH - T, M_TRANSITION+2]) mirror([0, 1, 0]) triBump(12, NOTCH, NOTCH*2);
                        translate([0,  Y_SWITCH - T, M_TRANSITION-2]) mirror([0, 1, 0]) triBump(12, 1.5, 3);
                    }                
                    cylinder(h=M_TRANSITION, d=30);
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
            transitionRing(false);
            heatSink();
        }
    }
}


module triBump(halfDX, dy, dz)
{
    translate([-halfDX, 0, 0]) {
        hull() {
            cube(size=[halfDX*2, EPS, dz]);
            translate([0, 0, dz/2]) cube(size=[halfDX*2, dy, EPS]);
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
}

module transitionRing(bars=true)
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
        if (bars) upperBars();
    }
}

module upperBars()
{
    DY = 12;
    M = M_BUTTRESS_0 + H_BUTTRESS * 13 - EPS;
    DZ = 1;
    L = M_TRANSITION - T_TRANSITION_RING - M + EPS2;

    translate([0, 1, M - DZ])
        cubePair(x=W_MC/2 + 1, size=[2, DY, L + DZ*2]);
}

M_MC_START = M_SPKR_RING + H_BUTTRESS;
BAR_HEIGHT = 8;

/*
module horn()
{
    translate([0, 0, M_TRANSITION]) {
        intersection() {
            cylinder(h=10, d=D_AFT); 
            difference() {
                hull() {
                    translate([-HORN_WIDTH/2, Y_SWITCH, 0]) {
                        cube(size=[HORN_WIDTH, 7, 2]);
                    }    
                    translate([-HORN_BASE_WIDTH/2, Y_SWITCH, 0]) {
                        cube(size=[HORN_BASE_WIDTH, 0.1, 2]);
                    }    
                }
                translate([-HORN_BASE_WIDTH/2, R_FORWARD, 0]) {
                    rotate([45, 0, 0]) {
                        cube(size=[HORN_BASE_WIDTH, 20, 10]);
                    }    
                }
                translate([0, 0, M_PORT_CENTER - M_TRANSITION]) {
                    rotate([-90, 0, 0]) {
                        cylinder(h=20, d=10.5);
                    }
                }
                translate([0, 0, -10]) cylinder(h=20, d=D_FORWARD);
            }
        }
    }    
}
*/

FLATTEN = 1.8;

if (DRAW_FRONT) {
    union() {
        ledHolder();
        switchAndPortHolder();
        forwardRail();
    }
}

if (DRAW_BAT) {
    D = 1.5;

    color("olive") 
    difference() {
        intersection() {
            innerTube();
            union() {
                translate([0, 0, M_BUTTRESS_3]) 
                    buttress(wiring=false, clip=true, circle=2.5, h=7);
                translate([0, 0, M_BUTTRESS_4]) 
                    buttress(battery=false, trough=10, wiring=false, clip=true);

                M = M_BUTTRESS_0 + H_BUTTRESS * 13 - EPS;
                L = M_TRANSITION - T_TRANSITION_RING - M + EPS2;

                upperBars();
                for(i=[0:1]) {
                    translate([0, 0, M_BUTTRESS_4 + H_BUTTRESS*(2 + 2*i)]) {
                        intersection() {
                            //translate([-10, 1, 0]) cube(size=[20, 100, H_BUTTRESS]);
                            translate([-13, 1, 0]) cube(size=[26, 100, H_BUTTRESS]);
                            difference() {
                                cylinder(h=H_BUTTRESS, d=D_AFT);
                                cylinder(h=H_BUTTRESS, d=D_AFT-10);
                                translate([-5, 0, 0]) cube(size=[10, 100, H_BUTTRESS]);
                            }                        
                        }
                    }
                }
            }
        }
        translate([-20, -10, M_BUTTRESS_4-EPS]) cube(size=[40, 11, H_BUTTRESS+EPS2]);

        // Flatten the bottom for printing.
        translate([-20, -D_AFT_RING/2, M_WAY_BACK]) cube(size=[40, FLATTEN, H_FAR]);

        OFFSET = 20;
        translate([-W_MC/2, Y_MC - OFFSET, M_SPKR_RING + H_BUTTRESS]) {
            cube(size=[W_MC, H_MC + OFFSET - 1, 100]);
        }
    }
}

if (DRAW_BACK) {
    difference() {
         union() {
            transitionRing();
            
            intersection() 
            {
                innerTube();
                union() {
                    OFFSET = 2;
                    // Stop bars, left & right
                    translate([0, -R_AFT, M_MC_START + Z_MC_35 + OFFSET- 4])
                        cubePair(x=W_MC/2 - 3, size=[2, BAR_HEIGHT, 6]);

                    // Stop bar, center.
                    translate([-W_MC/2, -R_AFT, M_MC_START + Z_MC_35 + OFFSET]) {
                        cube(size=[W_MC, BAR_HEIGHT, 2]);
                    }

                    translate([0, 0, M_SPKR_RING]) mirror([1, 0, 0])
                        shelfBeam(M_TRANSITION - T_TRANSITION_RING + EPS - M_SPKR_RING, BEAM_WIDTH);
                    translate([0, 0, M_SPKR_RING]) 
                        shelfBeam(M_TRANSITION - T_TRANSITION_RING + EPS - M_SPKR_RING, BEAM_WIDTH);

                    // Bottom rails that hold up microcontrolller
                    RAIL_Z = M_TRANSITION - T_TRANSITION_RING + EPS - M_SPKR_RING;
                    translate([0, 0, M_SPKR_RING]) {
                        translate([0, RAIL_DY - 20, 0])
                            cubePair(x=W_MC/2 - MC_RAIL, size=[MC_RAIL, 20, RAIL_Z]);
                    }
                }
            }
            translate([0, 0, M_BUTTRESS_0]) buttress(trough = 8, wiring=false);

            speakerHolder();

            //horn();

            for(i=[0:5]) {
                translate([0, 0, M_SPKR_RING + H_BUTTRESS * (3 + 2 * i)]) 
                    buttress(leftWiring=false, rightWiring=false, trough=W_MC, clip=true, bridge=true);
            }

            // Hold the forward PCB
            hull() {
                translate([W_MC/2 + 1, -5.5, M_BUTTRESS_4 + H_BUTTRESS + 3]) 
                    cube(size=[2, 4.5, 12]);
                translate([W_MC/2, -5.5, M_BUTTRESS_4 + H_BUTTRESS + 2]) 
                    cube(size=[4, 1, 14]);
            }
            mirror([1,0,0]) 
            hull() {
                translate([W_MC/2 + 1, -5.5, M_BUTTRESS_4 + H_BUTTRESS + 3]) 
                    cube(size=[2, 4.5, 12]);
                translate([W_MC/2, -5.5, M_BUTTRESS_4 + H_BUTTRESS + 2]) 
                    cube(size=[4, 1, 14]);
            }


            // Special connectors from the back buttress to the lock ring.
            translate([0, 0, M_SPKR_RING]) {
                intersection() {
                    tube(H_BUTTRESS, D_SPKR_INNER/2, D_AFT/2);
                    union() {
                        translate([W_MC/2, 5, 0]) 
                            cube(size=[BEAM_WIDTH, 10, H_BUTTRESS]);
                        mirror([1,0,0]) translate([W_MC/2, 5, 0]) 
                            cube(size=[BEAM_WIDTH, 10, H_BUTTRESS]);
                    }
                    translate([-20, 1.8, 7]) rotate([-45, 0, 0]) cube(size=[40, 14, 14]);
                }
            }

        }
        upperBars();

        // Flatten the bottom for printing.
        translate([-20, -D_AFT_RING/2, M_WAY_BACK]) cube(size=[40, FLATTEN, H_FAR]);
        
        // Take out a chunk for access to the USB port.
        X_USB = 14; // 10 to tight fit
        Y_USB = 9;
        Z_USB = 20;
        translate([-X_USB/2, -R_AFT, M_POMMEL_BACK]) cube(size=[X_USB, Y_USB, Z_USB]);
    }
}

*switch();
*color("yellow") heatSink();
*color("yellow") speaker();
*color("yellow") speakerHolder();
*color("yellow") teensy35();
*buttress();
