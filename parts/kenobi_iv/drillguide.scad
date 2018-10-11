include <dim.scad>
use <../shapes.scad>

$fn = 180;

difference()
{
    union()
    {
        tube(h=DZ_MID_OUTER, di=D_MID_OUTER + 0.2, do=D_MID_OUTER + 4);
        difference() {
            DX = 42;
            DY = 26;
            translate([-DX/2, -DY/2, 0]) cube(size=[DX, DY, DZ_MID_OUTER]);
            cylinder(h=DZ_MID_OUTER, d=D_MID_OUTER+3);
        }
    }
    hull() {
        translate([0, 0, DZ_MID_OUTER/2 - (22 - 11)/2])
            rotate([-90, 0, 0])
                cylinder(h=100, d=11);
        translate([0, 0, DZ_MID_OUTER/2 + (22 - 11)/2])
            rotate([-90, 0, 0])
                cylinder(h=100, d=11);
    }
    translate([0, 0, DZ_MID/2 + M_MID_BACK]) rotate([0, 90, 0]) cylinder(h=100, d=3.0);
    translate([0, 0, DZ_MID/2 + M_MID_BACK]) rotate([0, -90, 0]) cylinder(h=100, d=3.0);
}


