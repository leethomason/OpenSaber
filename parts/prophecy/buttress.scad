include <dim.scad>
use <beam.scad>

EPS = 0.01;
EPS2 = EPS * 2;
BATTERY_CUTOUT		= 18;	// 12 to hold. This value set for drop in.
TROUGH 				= 12;

module battery(h)
{
	BATTERY_Y = R_AFT - R_BATTERY;

	translate([0, BATTERY_Y, 0]) {
	    cylinder(h=h, d=D_BATTERY);

	    translate([-BATTERY_CUTOUT/2, 0, 0]) {
	        cube(size=[BATTERY_CUTOUT, 40, h]);
	    }
	}
}

module circuitry(h, deltaY, wideWing)
{
	OFFSET  = 10;

	translate([-W_MC/2, Y_MC-OFFSET, 0]) cube(size=[W_MC, H_MC+OFFSET, h]);
	if(wideWing >= 0) {
		translate([-(W_WING + wideWing)/2, Y_MC + H_MC, 0]) cube(size=[W_WING + wideWing, H_WING + deltaY, h]);
	}
}

module shelfRail(h, right=true, t=3)
{
	T = 3;
	intersection() {
		cylinder(h=h, d=D_AFT);
		if (right)
			mirror([1, 0, 0]) translate([W_MC/2, Y_MC + H_MC - T, 0]) cube(size=[t, T, h]);
		else
			translate([W_MC/2, Y_MC + H_MC - T, 0]) cube(size=[t, T, h]);
	}
}

module oneShelfBeam(h, fill, width)
{
	H = 8;
	translate([W_MC/2, Y_MC + H_MC - H, 0]) {
		if (fill)
			cube(size=[width, H, h]);
		else
			beam(width, H, h);
	}
	translate([W_MC/2, Y_MC + H_MC - H - 10, 0]) cube(size=[width, 10, h]);
}

module shelfBeam(h, right=true, width=4) {
	intersection() {
		cylinder(h=h, d=D_AFT);
		if (right) 
			mirror([1, 0, 0]) oneShelfBeam(h, false, width);
		else
			oneShelfBeam(h, false, width);
	}
}

module buttress(	h=H_BUTTRESS,
					leftWiring=true, 
					rightWiring=true, 
					battery=true, 
					mc=true, 
					mcDeltaY=0, 
					trough=0, 
					wideWing=0,
					highHoles=0,
					highBoost=[])
{
	difference() {
		cylinder(h=h, d=D_AFT);	

		// Battery
		if (battery) {
			translate([0, 0, -EPS]) battery(h + EPS2);
		}

		// Board
		if (mc) {
			translate([0, 0, -EPS]) circuitry(h + EPS2, mcDeltaY, wideWing);
		}

		if (trough != 0) {
			T = trough < 0 ? TROUGH : trough;
            translate([-T/2, -12, -EPS]) {
                cube(size=[T, 40, h + EPS2]);
            }
		}

	    // Wiring holes
	    X_WIRING =  11.5;
	    Y_WIRING = -6.5;
	    D 		 =  2;

	    Y_WIRING_B = -8.0;

	    if (leftWiring) {
	    	hull() {
			    translate([ X_WIRING, Y_WIRING, -EPS]) cylinder(h=h + EPS2, d=D);
			    translate([ X_WIRING, Y_WIRING_B, -EPS]) cylinder(h=h + EPS2, d=D);	    	
	    	}
	    }
	    if (rightWiring) {
	    	hull() {
			    translate([-X_WIRING, Y_WIRING, -EPS]) cylinder(h=h + EPS2, d=D);
			    translate([-X_WIRING, Y_WIRING_B, -EPS]) cylinder(h=h + EPS2, d=D);
	    	}
	    }
	    if (highHoles) {
		    hull() {
			    translate([13, 3, -EPS]) cylinder(h=h + EPS2, d=3);
			    translate([13, 0, -EPS]) cylinder(h=h + EPS2, d=3);
			    translate([13-highHoles, 3 + highBoost, -EPS]) cylinder(h=h + EPS2, d=3);
			    translate([13-highHoles, 0, -EPS]) cylinder(h=h + EPS2, d=3);
		    }
		    hull() {
			    translate([-13, 3, -EPS]) cylinder(h=h + EPS2, d=3);
			    translate([-13, 0, -EPS]) cylinder(h=h + EPS2, d=3);
			    translate([-13+highHoles, 3 + highBoost, -EPS]) cylinder(h=h + EPS2, d=3);
			    translate([-13+highHoles, 0, -EPS]) cylinder(h=h + EPS2, d=3);
		    }
	    }
	}
}

