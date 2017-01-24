use <../threads.scad>
use <../shapes.scad>
use <../vents.scad>
include <dim.scad>
use <buttress.scad>

INCHES_TO_MM = 25.4;
MM_TO_INCHES = 1 / 25.4;
$fn = 90;
EPS = 0.01;
EPS2 = EPS * 2;

module rail(r)
{
    H = M_TRANSITION - M_RAIL_START - T_TRANSITION_RING;

    difference() {
        intersection()
        {
            innerTube();
            rotate([0, 0, r]) {
                translate([R_INNER - X_RAIL/2, 0, M_RAIL_START + H/2]) {
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


difference() {
    union() {
        *union() {
            rail(RAIL_ANGLE_0);
            rail(RAIL_ANGLE_1);
            rail(RAIL_ANGLE_2);
            rail(RAIL_ANGLE_3);
        }
        translate([0, 0, M_BUTTRESS_0]) buttress(mc=false, trough = 8);
        translate([0, 0, M_BUTTRESS_1]) buttress(mcDeltaY=20
        );
        translate([0, 0, M_BUTTRESS_2]) buttress(mcDeltaY=20);
        translate([0, 0, M_BUTTRESS_3]) buttress(mc=false, trough=8);
        translate([0, 0, M_BUTTRESS_4]) buttress(battery=false, trough=12, mc=false);
    }
    // Flatten the bottom for printing.
    translate([-20, -D_INNER/2, M_WAY_BACK]) cube(size=[40, 1.8, H_FAR]);
    // Take out a chunk for access to the USB port.
    X_USB = 11; // 10 to tight fit
    Y_USB = 6;
    Z_USB = 20;
    translate([-X_USB/2, -R_INNER, M_0]) cube(size=[X_USB, Y_USB, Z_USB]);
}
