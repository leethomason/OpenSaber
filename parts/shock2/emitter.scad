include <dim.scad>
use <../shapes.scad>
use <vents.scad>

// LED holder for Shock LE
// Designed for Dynamic (LED Supply) heatsink

LOCK_RING = false;
FACES = 180;
EPSILON = 0.1;

// Inches. I hate imperial.
D_LED       = 20.000;                 // hole where the light shines.
D_HEATSINK  = 1.000 * INCHES_TO_MM;

H_RING      = 0.280 * INCHES_TO_MM;    // measured at 279
H_HEATSINK  = 0.45 + 0.890 * INCHES_TO_MM;
H_OUTER     = 0.700 * INCHES_TO_MM;    // correct. height below ring.

H_CONNECTOR = 13.4;
H_CAP       = 3;
D_INNER_CAP = 0.5 + 0.800 * INCHES_TO_MM;                       // end cap holds the emitter in place

H_TEETH = 2;

// Top part (emitter)
module emitter()
{
    difference() {
        union() {
            // Top cap
            translate([0, 0, H_OUTER]) {
                difference() {
                    if (LOCK_RING)
                        cylinder(h=H_RING, r=D_EMITTER_RING/2, $fn=FACES);
                    else
                        cylinder(h=H_RING, r=D_INNER/2, $fn=FACES);
                    cylinder(h=H_RING, r=D_LED/2, $fn=FACES);
                }
            };
        
            // LED
            cylinder(h=H_OUTER, r=D_INNER/2, $fn=FACES);
        }
        cylinder(h=H_HEATSINK, r=D_HEATSINK/2, $fn=FACES);

        // Vents / decoration / volume reduction
        translate([0, 0, 16]) {
            rotate([180, 0, 0]) {
                vent1(8, 12, 8, 20);
            }
        }

        for(r=[0:(N_TEETH-1)]) {
            rotate([0, 0, r*TEETH_ANGLE]) {
                lock(H_TEETH);
            }   
        }

    }
}


module emitterBase()
{
    // Bottom part (connects to mainRing)
    color("gray") {
        for(r=[0:(N_TEETH-1)]) {
            rotate([0, 0, r*TEETH_ANGLE]) {
                intersection() {
                    tube(H_TEETH, D_HEATSINK/2, D_INNER/2);
                    lock(H_TEETH);
                }
            }
        }
        
        intersection()
        {
            translate([0, 0, -H_CONNECTOR]) {
                cylinder(h=H_CONNECTOR, r=D_INNER/2, $fn=FACES);
            }
            union() {
                // End cap.
                translate([0, 0, -H_CAP]) {
                    tube(H_CAP, D_INNER_CAP/2, D_INNER/2);
                }
            }                 
        }
    }
}
