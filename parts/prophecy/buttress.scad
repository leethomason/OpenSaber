include <dim.scad>

EPS = 0.01;
EPS2 = EPS * 2;

module battery(h)
{
//	BATTERY_Y = 6;
	BATTERY_Y = R_AFT - R_BATTERY;

	translate([0, BATTERY_Y, 0]) {
	    cylinder(h=h, d=D_BATTERY);

	    CUTOUT = 12;
	    translate([-CUTOUT/2, 0, 0]) {
	        cube(size=[CUTOUT, 40, h]);
	    }
	}
}

module circuitry(h)
{
	W_MC 	= 18 + 1;
	H_MC    = 9;
	Y_MC    = -13;

	W_JOINT = 4;
	H_JOINT = 4;
	X_JOINT = 7;
	Y_JOINT = -5;

	W_WING  = 5;
	H_WING  = 8;
	X_WING  = 9.5;
	Y_WING  = -2;

	translate([-W_MC/2, Y_MC, 0]) cube(size=[W_MC, H_MC, h]);

	hull() {
		translate([X_JOINT, Y_JOINT, 0]) cube(size=[W_JOINT, H_JOINT, h]);
		mirror([1, 0, 0]) translate([X_JOINT, Y_JOINT, 0]) cube(size=[W_JOINT, H_JOINT, h]);
	}
	hull() {
		translate([X_WING, Y_WING, 0]) cube(size=[W_WING, H_WING, h]);
		mirror([1, 0, 0]) translate([X_WING, Y_WING, 0]) cube(size=[W_WING, H_WING, h]);
		}
//	translate([-W/2, -14 - OFFSET, 0]) {
//		cube(size=[W, H + OFFSET, h]);
//	}
//	translate([])
}

module buttress(leftWiring=true, rightWiring=true)
{
	difference() {
		cylinder(h=H_BUTTRESS, d=D_AFT);	

		// Battery
		translate([0, 0, -EPS]) battery(H_BUTTRESS + EPS2);

		// Board
		translate([0, 0, -EPS]) circuitry(H_BUTTRESS + EPS2);

	    // Wiring holes
	    if (leftWiring) {
		    //translate([ 11.5, 0, -EPS]) cylinder(h=H_BUTTRESS + EPS2, d=5);
		    translate([ 12.5, -5.5, -EPS]) cylinder(h=H_BUTTRESS + EPS2, d=3.5);
	    }
	    if (rightWiring) {
		    //translate([-11.5, 0, -EPS]) cylinder(h=H_BUTTRESS + EPS2, d=5);
		    translate([-12.5, -5.5, -EPS]) cylinder(h=H_BUTTRESS + EPS2, d=3.5);
	    }
	}
}

