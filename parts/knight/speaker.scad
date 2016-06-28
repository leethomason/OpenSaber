use <../threads.scad>
use <../shapes.scad>
include <dim.scad>

/*
	3 sections:
	TUBE_COUPLE
	SPKR_METAL
	SPKR_PLASTIC
*/

TUBE_COUPLE_H  = 3.5;
TUBE_COUPLE_D  = 25.4;
SPKR_METAL_H   = 8;
SPKR_METAL_D   = 23.6;	
SPKR_PLASTIC_H = 2.7;
SPKR_PLASTIC_D = 28;

difference() 
{
	union() {
		tube(TUBE_COUPLE_H, D_AFT/2, TUBE_COUPLE_D/2);
		translate([0,0,TUBE_COUPLE_H]) {
			tube(SPKR_METAL_H, D_AFT/2, SPKR_METAL_D/2);
			translate([0,0,SPKR_METAL_H]) {
				tube(SPKR_PLASTIC_H, D_AFT/2, SPKR_PLASTIC_D/2);
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