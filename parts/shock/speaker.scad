use <../threads.scad>
use <../shapes.scad>
include <dim.scad>

/*
	2 sections:
	SPKR_METAL
	SPKR_PLASTIC
*/

module speaker20() {
    $fn=90;

    SPKR_METAL_H   = 6;
    SPKR_METAL_D   = 18;    
    SPKR_PLASTIC_H = 2;
    SPKR_PLASTIC_D = 20;
    SPKR_H = SPKR_METAL_H + SPKR_PLASTIC_H;
    HOLE_X = 2;
    HOLE_Y = 4;

    difference() 
    {
        difference() {
            cylinder(d=D_AFT, h=SPKR_H);
            cylinder(d=SPKR_METAL_D, h=SPKR_H);
            translate([0, 0, SPKR_METAL_H]) cylinder(d=SPKR_PLASTIC_D, 10);
        }
        ROTATIONS = [0, 180];
        for(r=ROTATIONS) {
            rotate([0, 0, r]) {
                translate([SPKR_METAL_D/2-1, -HOLE_Y/2, 0]) {
                    cube(size=[HOLE_X+1, HOLE_Y, 10]);
                }
            }
        }
    }    
}

speaker20();

