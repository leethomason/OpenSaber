include <dim.scad>
use <../shapes.scad>

$fn = 60;

Y_DOWN = 4.0;
H_BUTTON = 5.0;
H_PIN = 4.0;

module body()
{
    difference() {
        translate([0, 0, -20]) tube(h=40, do=D_OUTER, di=D_INNER);
        rotate([-90, 0, 0]) cylinder(h=20, d=12.0);
    }
}

difference() {
    translate([0, D_INNER/2 - Y_DOWN, 0]) rotate([-90, 0, 0]) cylinder(h=H_BUTTON, d=16.0);
    body();
    translate([0, D_INNER/2 - Y_DOWN, 0]) rotate([-90, 0, 0]) cylinder(h=H_PIN, d1=4.0, d2=0.0);
}
