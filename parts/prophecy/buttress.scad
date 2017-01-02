include <dim.scad>

EPS = 0.01;
EPS2 = EPS * 2;

module battery(h)
{
	translate([0, R_AFT - R_BATTERY, 0]) {
	    cylinder(h=h, d=D_BATTERY);

	    CUTOUT = 12;
	    translate([-CUTOUT/2, 0, 0]) {
	        cube(size=[CUTOUT, 40, h]);
	    }
	}
}

module circuitry(h)
{
	W       = 18 + 1;
	W_INNER = 15; 
	H       = 10.5;
	H_INNER = 7;
	OFFSET  = 10;

	translate([-W/2, -14 - OFFSET, 0]) {
		cube(size=[W, H + OFFSET, h]);
	}
}

module buttress(leftWiring=true, rightWiring=true, notch=true)
{
	difference() {
		cylinder(h=H_BUTTRESS, d=D_AFT);	

		// Battery
		translate([0, 0, -EPS]) battery(H_BUTTRESS + EPS2);

		// Board
		translate([0, 0, -EPS]) circuitry(H_BUTTRESS + EPS2);

		if (notch) {
			// Rail notches
		    R = [RAIL_ANGLE_0, RAIL_ANGLE_1];

		    for(r=R) {
		        rotate([0, 0, r]) {
	                translate([-W_RAIL/2 - RAIL_PAD/2, R_AFT - RAIL_INNER_NOTCH, 0]) {
	                   cube(size=[W_RAIL + RAIL_PAD, 20, H_BUTTRESS]);
	                }
		        }
		    }
		}

	    // Wiring holes
	    if (leftWiring) {
		    translate([ 11.5, 0, -EPS]) cylinder(h=H_BUTTRESS + EPS2, d=5);
		    translate([ 12.5, -5.5, -EPS]) cylinder(h=H_BUTTRESS + EPS2, d=3.5);
	    }
	    if (rightWiring) {
		    translate([-11.5, 0, -EPS]) cylinder(h=H_BUTTRESS + EPS2, d=5);
		    translate([-12.5, -5.5, -EPS]) cylinder(h=H_BUTTRESS + EPS2, d=3.5);
	    }
	}
}

