
use <../shapes.scad>

INCHES_TO_MM = 25.4;
MM_TO_INCHES = 1/ INCHES_TO_MM;

D_SABER = 1.3 * INCHES_TO_MM;		// FIXME
D_OUTER = D_SABER + 20;				// FIXME
D_TUBE = 5;							// FIXME
D_BIT  = 4;							// FIXME
H = 30;						
H_SLEEVE = 3;						// FIXME

difference() 
{
	tube(H, D_OUTER/2, D_SABER/2);
	translate([0,0,H/2]) {
		rotate([0,90,0]) {
			for(r=[0:3]) {
				rotate([r*20,0,0]) {
					translate([0,0,D_SABER/2 + H_SLEEVE]) {
						cylinder(h=H, r=D_TUBE/2,$fn=28);
					}
					cylinder(h=H, r=D_BIT/2, $fn=28);
				}
			}
		}
		translate([-100,-10,-D_TUBE/2]) {
			cube([100,20,D_TUBE]);
		}
	}
}
