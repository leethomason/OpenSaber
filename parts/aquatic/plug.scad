use <../shapes.scad>
use <../vents.scad>

$fn=90;

H_BOTTOM = 2;
H_OUTER = 14;
H_LIP = 2;

cylinder(h=H_BOTTOM, d=25.4);

difference() {
    union() {
        tube(H_OUTER, 22/2, 25.4/2);
        tube(H_OUTER+H_LIP, 10, 11);
    }
    translate([0, 0, 2.5]) vent2(5, 9, 6, 20);
}
