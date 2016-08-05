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
SPKR_METAL_H   = 8 + 2.8;
SPKR_METAL_D   = 23.6;	
SPKR_PLASTIC_H = 2.7;
SPKR_PLASTIC_D = 28;
TRAN     	   = (TUBE_COUPLE_D - SPKR_METAL_D)/2;

H_PORT              = 8;
OFFSET              = R_AFT - H_PORT;

//intersection()
{
	difference() 
	{
		union() {
			tube(TUBE_COUPLE_H + TRAN, TUBE_COUPLE_D/2, D_AFT/2);
			translate([0, 0, TUBE_COUPLE_H]) {
				shelf(TUBE_COUPLE_D	/2, TRAN, SPKR_METAL_D/2);
				translate([0, 0, TRAN]) {
					tube(SPKR_METAL_H - TRAN, SPKR_METAL_D/2, D_AFT/2);
					translate([0, 0, SPKR_METAL_H - TRAN]) {
						tube(SPKR_PLASTIC_H, SPKR_PLASTIC_D/2, D_AFT/2);
					}
				}
			}
		}
		for(r=[0:1]) {
            S = 8;
            S2 = 4;
			rotate([0,0,r*180]) {
				translate([D_AFT/2 - S, -S/2, TUBE_COUPLE_H + 2]) {
					cube(size=[S+1, S, 20], center=false);
				}
                translate([TUBE_COUPLE_D/2 - S2, -S/2, -0.1]) {
                    cube(size=[4, S, 20]);
                }
			}
		}
        for(r2=[0:1]) {
            rotate([0, 0, 90 + 180*r2]) {
                translate([OFFSET,-100,0]) {
                    cube(size=[200, 200, TUBE_COUPLE_H]);
                }
                pinsAtTube(R_AFT, 10, 1, 1, true);
            }
        }
	}
//	cube(size=[100,100,100]);
}