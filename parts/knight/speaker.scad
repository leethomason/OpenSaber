use <../threads.scad>
use <../shapes.scad>
include <dim.scad>

/*
	3 sections:
	TUBE_COUPLE
	SPKR_METAL
	SPKR_PLASTIC
*/

// ALL FIXME
OUTER_D = 30;
TUBE_COUPLE_H = 6;
TUBE_COUPLE_D = 25.4;
SPKR_METAL_H = 3;
SPKR_METAL_D = 26.5;	
SPKR_PLASTIC_H = 4;
SPKR_PLASTIC_D = 28;

difference() 
{
	union() {
		tube(TUBE_COUPLE_H, OUTER_D/2, TUBE_COUPLE_D/2);
		translate([0,0,TUBE_COUPLE_H]) {
			tube(SPKR_METAL_H, OUTER_D/2, SPKR_METAL_D/2);
			translate([0,0,SPKR_METAL_H]) {
				tube(SPKR_PLASTIC_H, OUTER_D/2, SPKR_PLASTIC_D/2);
			}
		}
	}
	for(r=[0:1]) {
		rotate([0,0,r*180]) {
			translate([SPKR_PLASTIC_D/2-4, -2,-1]) {
				cube(size=[4,4,20], center=false);
			}
		}
	}
}