use <../threads.scad>
use <../shapes.scad>
include <dim.scad>

module vent1(w, h1, h2, extrusion)
{
	rotate([90, 0, 90]) {
		linear_extrude(height=extrusion) {
			polygon([[-w/2,0], [w/2,0], [w/2,h1], [0,h2], [-w/2,h1]]);
		}
	}
}

module vent2(w, h, extrusion)
{
	rotate([90, 0, 90]) {
		linear_extrude(height=extrusion) {
			polygon([[0,0], [w/2,w/2], [w/2, h-w/2], [0, h], [-w/2, h-w/2], [-w/2, w/2]]);
		}
	}
}

module vent3(w, h1, h2, extrusion)
{
	W = w / 2;
	IW = W * 0.62;
	IH = h1 * 0.38;

	rotate([90, 0, 90]) {
		linear_extrude(height=extrusion) {
			difference() {
				polygon([[-W,0], [W,0], [W,h1], [0,h2], [-W,h1]]);
				polygon([[-IW, 0], [IW, 0], [IW, IH], [-IW, IH]]);
			}
		}
	}
}

module vent4(h, r, extrusion)
{
	theta = 30;
	alpha = 16;
	w = sin(alpha) * r;

	for(i=[0:11]) {
		rotate([0, 0, 30*i]) {
			rotate([90, 0, 90]) {
				linear_extrude(height=extrusion) {
					polygon([[-w/2, 0], [w/2, 0], [0, h] ]);
				}
			}
		}
		rotate([0, 0, 30*i + 15]) {
			rotate([90, 0, 90]) {
				linear_extrude(height=extrusion) {
					polygon([[-w/2, h], [0, 0], [w/2, h] ]);
				}
			}
		}
	}
}


difference() 
{
	tube(16, D_INNER/2 - 2, D_INNER/2);	
	*for(r=[0:5]) {
		rotate([0, 0, 60*r]) {
			translate([0, 0, 2]) {
				//vent1(6, 7, 12, 40);
				//vent2(6, 12, 40);
				vent3(6, 7, 12, 40);
			}
		}
	}
	translate([0, 0, 3]) {
		vent4(10, D_INNER/2, 20);
	}
}
