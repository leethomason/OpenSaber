use <../threads.scad>
use <../shapes.scad>
include <dim.scad>

FORE_H = 4;
AFT_H = 4;
SPKR_SHELF_H = 3;

D_SPEAKER = 28;	// FIXME

// FIXME...account for outer vs. inner speaker properly

difference() 
{
	union() {
		tube(FORE_H, D_AFT/2, 25.4/2);
		translate([0,0,FORE_H]) {
			tube(AFT_H, D_AFT/2, D_SPEAKER/2);
			//translate([0,0,SPKR_SHELF_H]) {
			//	tube()
			//}
		}
	}
	for(r=[0:1]) {
		rotate([0,0,r*180]) {
			translate([D_SPEAKER/2 + 1.8, 0,0]) {
				cube(size=[4,4,20], center=true);
			}
		}
	}
}