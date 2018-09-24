include <dim.scad>
use <util.scad>

$fn = 90;


difference() {
    translate([-BOX_DX/2, 0, 0]) cube(size=[BOX_DX, D_MID_OUTER/2 + BOX_Y, DZ_MID_OUTER]);
    translate([-PLATE_DX/2, 0, (DZ_MID_OUTER - PLATE_DZ)/2]) cube(size=[PLATE_DX, 100, PLATE_DZ]);
    cylinder(h=DZ_MID_OUTER, d=D_MID_OUTER);
}

//inner();