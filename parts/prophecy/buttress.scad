include <dim.scad>

EPS = 0.01;
EPS2 = EPS * 2;

module buttress()
{
	difference() {
		cylinder(h=H_BUTTRESS, d=D_AFT);	

		// Battery
		translate([0, R_AFT - R_BATTERY, -EPS]) {
			cylinder(h=H_BUTTRESS + EPS*2, d=D_BATTERY);

			CUTOUT = 12;

			translate([-CUTOUT/2, 0, 0]) {
				cube(size=[CUTOUT, 20, H_BUTTRESS + EPS2]);
			}
		}

		// Board
		W = 18 + 1; //0.7 * 25.4;
		W_INNER = 15; 
		H = 10.5; //4;
		H_INNER = 7;

		OFFSET = 10;

		translate([-W/2, -14 - OFFSET, -EPS]) {
			cube(size=[W, H + OFFSET, H_BUTTRESS + EPS2]);
		}

		// Rail notches
	    R = [RAIL_ANGLE_0, RAIL_ANGLE_1];

	    for(r=R) {
	        rotate([0, 0, r]) {
                translate([-W_RAIL/2 - RAIL_PAD/2, R_AFT - RAIL_INNER_NOTCH, 0]) {
                   cube(size=[W_RAIL + RAIL_PAD, 20, H_BUTTRESS]);
                }
	        }
	    }

	    // Wiring holes
	    translate([ 11.5, 0, -EPS]) cylinder(h=H_BUTTRESS + EPS2, d=5);
	    translate([-11.5, 0, -EPS]) cylinder(h=H_BUTTRESS + EPS2, d=5);
	    translate([ 12.5, -5.5, -EPS]) cylinder(h=H_BUTTRESS + EPS2, d=3.5);
	    translate([-12.5, -5.5, -EPS]) cylinder(h=H_BUTTRESS + EPS2, d=3.5);
	}
}

