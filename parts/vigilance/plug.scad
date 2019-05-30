use <../shapes.scad>
use <../vents.scad>

$fn=90;
cylinder(h=2, d=25.4);

difference() {
    tube(22, 22/2, 25.4/2);
    translate([0, 0, 4]) vent2(5, 16, 6, 20);
}
