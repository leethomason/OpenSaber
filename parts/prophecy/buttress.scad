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
		W = 0.7 * 25.4;
		H = 6;
		translate([-W/2, -12, -EPS]) {
			cube(size=[W, H, H_BUTTRESS + EPS2]);
		}
	}
}

