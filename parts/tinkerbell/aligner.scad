include <dim.scad>
use <../shapes.scad>

$fa = 6;   // minimum angle (default 12)
$fs = 0.8;  // minimum size (default 2)

DROP = 36.0;
H = 42;

difference() 
{
    union() {
        cylinder(h=H, d=40);
        W = 10;
        D2 = 42;
        translate([-W/2, -D2/2, 0]) cube(size=[W, D2, H]);
    }
    translate([0, 0, -1]) cylinder(h=100, d=37.2);
    translate([0, 0, DROP]) rotate([90, 0, 0]) cylinder(h=100, d=4.2);
    translate([0, 0, DROP]) rotate([-90, 0, 0]) cylinder(h=100, d=4.2);
}


