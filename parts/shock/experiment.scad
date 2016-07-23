use <../threads.scad>
use <../shapes.scad>
include <dim.scad>
use <vents.scad>

difference() 
{
	tube(16, D_INNER/2 - 2, D_INNER/2);	
	for(r=[0:11]) {
		rotate([0, 0, 30*r]) {
			translate([0, 0, 2]) {
				//vent1(5, 7, 12, 40);
				//vent2(6, 12, 40);
				vent3(6, 7, 12, 40);
			}
		}
	}
	*translate([0, 0, 3]) {
		vent4(10, D_INNER/2, 20);
	}
}
