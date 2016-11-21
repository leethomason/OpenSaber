use <../threads.scad>
use <../shapes.scad>
use <../vents.scad>
include <dim.scad>
use <buttress.scad>

M_DOTSTAR_EDGE = M_DOTSTAR - X_DOTSTAR / 2;

INCHES_TO_MM = 25.4;
MM_TO_INCHES = 1 / 25.4;
$fn = 90;
EPS = 0.01;
EPS2 = EPS * 2;

H_HEAT_SINK_THREAD = 10.0;
D_HEAT_SINK_THREAD = 20.0;
H_VOL			   = 16;

DOTSTAR_WALL       = Y_DOTSTAR + 1;

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

module dotstarWiring()
{
    rotate([0, 0, 180]) {
    translate([0, -7, M_TRANSITION - T_TRANSITION_RING]) {
        cube(size=[R_FORWARD - DOTSTAR_WALL, 14, M_LED_HOLDER_BACK - M_TRANSITION + T_TRANSITION_RING + 10]);
    }
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
        X_PAD = 2;
        X_STRIP = X_DOTSTAR + X_PAD;
        Z_STRIP = DOTSTAR_SPACE * 4 + X_DOTSTAR;
        T = 3;

        translate([-X_DOTSTAR/2 - X_PAD/2, 0, 0]) {
            cube(size=[X_STRIP, T, Z_STRIP]);
        }
    }
}

module dotstarsPositioned(y, strip)
{
    rotate([0, 0, -90]) {
        translate([0, -10, M_DOTSTAR_EDGE]) {
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
        dotstarWiring();
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

    intersection() {
        innerTube();
        difference() {
            translate([-10, Y_SWITCH - T, M_TRANSITION - T_TRANSITION_RING]) {
                cube(size=[20, T, H]);
            }

            // switch
            translate([0, 0, M_SWITCH_CENTER]) {
                rotate([-90, 0, 0]) {
                    cylinder(h=20, d=D_SWITCH);
                }        
            }

            // Port
            translate([0, 0, M_PORT_CENTER + PORT_CENTER_OFFSET]) {
                rotate([-90, 0, 0]) {
                    cylinder(h=20, d=D_PORT);
                }
            }
            transitionRing();
        }
    }
}


module dotstarHolder() {
    intersection()
    {
        innerTube();

        rotate([0, 0, -90]) 
        {
            difference()
            {
                OFFSET = 34;
                translate([-10, -R_FORWARD, M_TRANSITION - T_TRANSITION_RING + OFFSET]) {
                    cube(size=[20, DOTSTAR_WALL, M_LED_HOLDER_BACK - M_TRANSITION + T_TRANSITION_RING - OFFSET]);
                }
                translate([0, -10, M_DOTSTAR_EDGE]) {
                    dotstars(10);
               }
               //transitionRing();
            }
        }
    } 
}

module transitionRing()
{
    translate([0, 0, M_TRANSITION - T_TRANSITION_RING]) {
        tube(T_TRANSITION_RING, D_FORWARD/2, D_AFT/2);
    }
}

*ledHolder();
*switch();
*switchAndPortHolder();
*dotstarHolder();
transitionRing();

*intersection()
{
    translate([0, 0, M_TRANSITION - T_TRANSITION_RING]) {
        cylinder(d=D_FORWARD, h=M_LED_HOLDER_BACK - M_TRANSITION + T_TRANSITION_RING);
    }
    translate([-4, -R_FORWARD, M_TRANSITION - T_TRANSITION_RING]) {
        cube(size=[8, 2.5, 100]);
    }
}

*translate([0, 0, M_TRANSITION - T_TRANSITION_RING]) {
    difference() {
        cylinder(h=M_LED_HOLDER_BACK - M_TRANSITION + T_TRANSITION_RING, d=D_FORWARD);
        cylinder(h=M_LED_HOLDER_BACK - M_TRANSITION + T_TRANSITION_RING, d=D_HEAT_SINK_THREAD);
        rotate([0, 0, 0])   lowerVent1(14, M_LED_HOLDER_BACK - M_TRANSITION, 20);
        rotate([0, 0, 90])  lowerVent1(14, 100, 20);
        rotate([0, 0, 180]) lowerVent1(14, 100, 20);
        rotate([0, 0, 270]) lowerVent1(14, M_LED_HOLDER_BACK - M_TRANSITION, 20);
    }
}

*color("yellow") heatSink();
*color("yellow") dotstarWiring();


*translate([0, 0, 80]) {
    buttress();
}