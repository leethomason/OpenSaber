include <dim.scad>

// LED holder for Shock LE
// Designed for Dynamic (LED Supply) heatsink

FACES = 180;
EPSILON = 0.1;

// Inches. I hate imperial.
D_RING      = 1.312 * INCHES_TO_MM;   // correct, outer ring holds it in place.
D_LED       = 0.750 * INCHES_TO_MM;   // correct. hole where the light shines.
D_HEATSINK  = 1.000 * INCHES_TO_MM;

H_RING      = 0.280 * INCHES_TO_MM;    // measured at 279
H_HEATSINK  = 0.89  * INCHES_TO_MM;
H_OUTER     = 0.700 * INCHES_TO_MM;    // correct. height below ring.


difference() {
    union() {
        // Top cap
        translate([0, 0, H_OUTER]) {
            difference() {
                cylinder(h=H_RING, r=D_RING/2, $fn=FACES);
                cylinder(h=H_RING, r=D_LED/2, $fn=FACES);
            }
        };
    
        // LED
        cylinder(h=H_OUTER, r=D_INNER/2, $fn=FACES);
    }
    cylinder(h=H_HEATSINK, r=D_HEATSINK/2, $fn=FACES);
}