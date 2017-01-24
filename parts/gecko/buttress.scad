include <dim.scad>

EPS = 0.01;
EPS2 = EPS * 2;
BATTERY_CUTOUT		= 18;	// 12 to hold. This value set for drop in.
TROUGH 				= 12;

module battery(h)
{
	BATTERY_Y = R_INNER - R_BATTERY;

	translate([0, BATTERY_Y, 0]) {
	    cylinder(h=h, d=D_BATTERY);

	    translate([-BATTERY_CUTOUT/2, 0, 0]) {
	        cube(size=[BATTERY_CUTOUT, 40, h]);
	    }
	}
}

module circuitry(h, deltaY, wing)
{
	W_MC 	= 18;
	H_MC    = 10;
	Y_MC    = -12;

	translate([-W_MC/2, Y_MC, 0]) cube(size=[W_MC, H_MC, h]);
	if (wing) {
		translate([-W_WING/2, Y_MC + H_MC, 0]) cube(size=[W_WING, H_WING + deltaY, h]);
	}
}

module buttress(	battery=true, 
					mc=true, 
					mcDeltaY=0, 
					trough=0, 
					wing=true)
{
	difference() {
		cylinder(h=H_BUTTRESS, d=D_INNER);	

		// Battery
		if (battery) {
			translate([0, 0, -EPS]) battery(H_BUTTRESS + EPS2);
		}

		// Board
		if (mc) {
			translate([0, 0, -EPS]) circuitry(H_BUTTRESS + EPS2, mcDeltaY, wing);
		}

		if (trough != 0) {
			T = trough < 0 ? TROUGH : trough;
            translate([-T/2, -20, -EPS]) {
                cube(size=[T, 40, H_BUTTRESS + EPS2]);
            }
		}
	}
}

