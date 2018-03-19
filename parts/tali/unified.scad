
use <../threads.scad>
use <../shapes.scad>
use <../vents.scad>
include <dim.scad>
use <buttress.scad>
use <beam.scad>

DRAW_BACK  = true;
DRAW_BAT   = true;

INCHES_TO_MM = 25.4;
MM_TO_INCHES = 1 / 25.4;
$fn = 40;
//$fn = 100;
EPS = 0.01;
EPS2 = EPS * 2;

H_HEAT_SINK_THREAD = 10.0;
D_HEAT_SINK_THREAD = 20.2;  // 20.4 is loose (PHA), 20.1 tight (PLA)

module innerTube()
{
    translate([0, 0, M_WAY_BACK]) {
        cylinder(d=D_AFT, h=M_TRANSITION - M_WAY_BACK);
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
            translate([0, 0, M_POMMEL_FRONT - 1 + SPKR_OFFSET]) {
                speaker(0, 0);
            }
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

module transitionRing()
{
    translate([0, 0, M_TRANSITION - T_TRANSITION_RING]) difference() {
        cylinder(h=T_TRANSITION_RING, d=D_AFT);
        cylinder(h=T_TRANSITION_RING, d=12);
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

module speakerBass28() 
{
    translate([0, 0, M_POMMEL_BACK]) {
        difference() {
            cylinder(h=M_POMMEL_FRONT - M_POMMEL_BACK+EPS, d=D_POMMEL);
            cylinder(h=M_POMMEL_FRONT - M_POMMEL_BACK+EPS2, d=D_SPKR_METAL);
            translate([0, 0, 1]) cylinder(h=H_SPKR_PLASTIC, d=D_SPKR_PLASTIC);

            translate([-20, -20, 0]) cube(size=[40, 13, 100]);
            translate([-4, -0, 0]) cube(size=[8, 40, 100]);
        }
    }

}

M_MC_START = M_SPKR_RING + H_BUTTRESS;
BAR_HEIGHT = 8;
FLATTEN = 1.8;

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
                            translate([-13, 1, 0]) cube(size=[26, 100, H_BUTTRESS]);
                            difference() {
                                cylinder(h=H_BUTTRESS, d=D_AFT);
                                cylinder(h=H_BUTTRESS, d=D_AFT-10);
                                translate([-5, 0, 0]) 
                                    cube(size=[10, 100, H_BUTTRESS]);
                            }                        
                        }
                    }
                }
            }
        }
        translate([-20, -10, M_BUTTRESS_4-EPS]) 
            cube(size=[40, 11, H_BUTTRESS+EPS2]);

        // Flatten the bottom for printing.
        translate([-20, -D_AFT_RING/2, M_WAY_BACK]) 
            cube(size=[40, FLATTEN, H_FAR]);

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
            speakerBass28();
            
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

                    // Power port.
                    POWER_T = 2;
                    translate([-POWER_X/2-POWER_T, -R_AFT + FLATTEN, M_TRANSITION - T_TRANSITION_RING
                        - POWER_Z - POWER_T]) {
                        difference() {
                            cube(size=[POWER_X+2*POWER_T, POWER_Y, POWER_Z+POWER_T]);
                            translate([POWER_T, 0, POWER_T])
                                cube(size=[POWER_X, POWER_Y, POWER_Z]);
                        }
                    }
                }
            }
            translate([0, 0, M_BUTTRESS_0]) buttress(trough = 8, wiring=false);

            speakerHolder();

            for(i=[0:5]) {
                translate([0, 0, M_SPKR_RING + H_BUTTRESS * (3 + 2 * i)]) 
                    buttress(leftWiring=false, rightWiring=false, trough=W_MC, clip=true, bridge=true);
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
        Z_USB = 30;
        translate([-X_USB/2, -R_AFT, M_POMMEL_BACK]) cube(size=[X_USB, Y_USB, Z_USB]);
    }
}


*color("yellow") heatSink();
*color("yellow") speaker();
*color("yellow") speakerHolder();
*color("yellow") translate([0, 0, M_SPKR_RING]) battery(false);
*color("green") translate([0, 0, M_SPKR_RING]) circuitry();

*color("orange") translate([0, 0, M_TRANSITION - T_TRANSITION_RING - 24]) {
    translate([0, 9, 0]) {
        // 7 segment FIXME
        //    10mm wide
        //    16mm long
        //    5 mm high + 2mm board = 7mm
        // switch
        //    8 mm ??
        translate([-5, 0, 0]) cube(size=[10, 7, 16+8]);
    }
}

