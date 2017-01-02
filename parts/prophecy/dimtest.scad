include <dim.scad>
use <../shapes.scad>
use <../vents.scad>

$fn = 90;

D_SPKR_INNER = D_SPKR_PLASTIC - 4;

intersection()
{
    translate([-20, -20, 0]) cube(size=[40, 14, 95]);
    
    union()
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

        difference() {
            union() {
                translate([0, 0, M_SPKR_RING]) tube(M_TRANSITION - M_SPKR_RING, D_SPKR_INNER/2, R_AFT);
                translate([0, 0, M_POMMEL_FRONT]) tube(M_PORT_CENTER - M_POMMEL_FRONT, R_FORWARD - 2, R_FORWARD);
            }
            translate([0, 0, M_POMMEL_FRONT + 2])rotate([0, 0, -30]) {
                vent1(6, M_PORT_CENTER - M_POMMEL_FRONT - 4, 8, 20, 6 );
            }

            // Test port sizes.
            translate([0, 0, 25]) rotate([90, 0, 0]) cylinder(h=20, d=7.7);
            translate([0, 0, 45]) rotate([90, 0, 0]) cylinder(h=20, d=7.8);
            translate([0, 0, 65]) rotate([90, 0, 0]) cylinder(h=20, d=7.9);
            translate([0, 0, 85]) rotate([90, 0, 0]) cylinder(h=20, d=8.0);
        }
    }
}