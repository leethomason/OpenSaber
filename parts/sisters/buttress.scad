include <dim.scad>
use <beam.scad>

EPS = 0.01;
EPS2 = EPS * 2;
BATTERY_CUTOUT		= 18;	// 12 to hold. This value set for drop in.
TROUGH 				= 12;

module battery(h)
{
	BATTERY_Y = R_AFT - R_BATTERY - BATTERY_BIAS;

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

module oneShelfBeam(h, fill, width)
{
	translate([W_MC/2, Y_MC + H_MC - BEAM_HEIGHT, 0]) {
		if (fill)
			cube(size=[width, BEAM_HEIGHT, h]);
		else
			beam(width, BEAM_HEIGHT, h);
	}
	translate([W_MC/2, Y_MC + H_MC - BEAM_HEIGHT - 10, 0]) cube(size=[width, 10, h]);
}

module shelfBeam(h, width=4, fill=false) {
	intersection() {
		cylinder(h=h, d=D_AFT);
		oneShelfBeam(h, fill, width);
	}
}

module buttress(	leftWiring=true, 
					rightWiring=true, 
					battery=true, 
					trough=0,
					clip=false,
					wiring=false,
					h=H_BUTTRESS,
					bridge=false)
{
	difference() {
		cylinder(h=h, d=D_AFT);	

		// Battery
		if (battery) {
			translate([0, 0, -EPS]) battery(h + EPS2);
		}

		if (trough != 0) {
			T = trough < 0 ? TROUGH : trough;
            translate([-T/2, -12, -EPS]) {
                cube(size=[T, 40, h + EPS2]);
            }
		}

		// microcontroller / beams
		translate([-W_MC/2, Y_MC + H_MC - 40 - 1, -EPS])
			cube(size=[W_MC, 40, h + EPS2]);
		translate([W_MC/2, Y_MC + H_MC - 40, -EPS])
			cube(size=[BEAM_WIDTH, 40, h + EPS2]);
		mirror([1, 0, 0]) translate([W_MC/2, Y_MC + H_MC - 40, -EPS])
			cube(size=[BEAM_WIDTH, 40, h + EPS2]);
		
		if (clip) {
			translate([W_MC/2 + BEAM_WIDTH - 1, -20, -EPS]) {
				cube(size=[20, 40, h + EPS2]);
			}
			mirror([1, 0, 0]) translate([W_MC/2 + BEAM_WIDTH - 1, -20, -EPS]) {
				cube(size=[20, 40, h + EPS2]);
			}
			CR = 12;
			translate([W_MC/2 + BEAM_WIDTH - 1 + CR, 0, -EPS]) {
				cylinder(h=h + EPS2, r=CR + 1);
			}
			mirror([1, 0, 0]) translate([W_MC/2 + BEAM_WIDTH - 1 + CR, 0, -EPS]) {
				cylinder(h=h + EPS2, r=CR + 1);
			}

		}

		if (wiring) {
			translate([9, -2.5, -EPS]) cylinder(d=4, h=h + EPS2);		
			mirror([1, 0, 0]) translate([9, -2.5, -EPS]) cylinder(d=4, h=h + EPS2);		
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
	}

    if (bridge) {
	   	hull() {
   		    translate([W_MC/2 + 1, 4, -H_BUTTRESS - EPS])
		   		cube(size=[2, 4, H_BUTTRESS + EPS2]);
   		    translate([W_MC/2 + 1, 0, 0])
		   		cube(size=[2, 4, EPS]);   	    	
   	    }
	   	mirror([1, 0, 0]) hull() {
   		    translate([W_MC/2 + 1, 4, -H_BUTTRESS - EPS])
		   		cube(size=[2, 4, H_BUTTRESS + EPS2]);
   		    translate([W_MC/2 + 1, 0, 0])
		   		cube(size=[2, 4, EPS]);   	    	
   	    }
    }
}

