include <dim.scad>
use <../shapes.scad>

$fn = 80;
EPS = 0.01;
H = 0.25 * 25.4;

module segment(x0, x1)
{
    hull() {
        translate([x0, 0, 0]) rotate([-90, 0, 0]) cylinder(h=10, d=3.175);
        translate([x1, 0, 0]) rotate([-90, 0, 0]) cylinder(h=10, d=3.175);
    }
}

module switchPlate()
{
difference() {
    translate([-DX_INSET/2, 0, 0]) 
        roundedRect(size=[DX_INSET, H, DZ_INSET], r=3.175/2, up="y");

    translate([0, -EPS, DZ_INSET/2 + BOLT_TO_PORT]) rotate([-90, 0, 0]) 
        cylinder(h=100, d=11.0); 

    translate([0, -EPS, DZ_INSET/2 + BOLT_TO_SWITCH]) rotate([-90, 0, 0]) 
        cylinder(h=100, d=D_BUTTON_HEAD); 

    S = 12.0;
    *translate([-S/2, H - 1.5, DZ_INSET/2 + BOLT_TO_SWITCH - S/2])
        roundedRect(size=[S, 100, S], r=3.175/2, up="y");
    hull() {
        translate([0, H - 1.5, DZ_INSET/2 + BOLT_TO_SWITCH + 1.0]) rotate([-90, 0, 0]) 
            cylinder(h=100, d=11.0); 
        translate([0, H - 1.5, DZ_INSET/2 + BOLT_TO_SWITCH - 2.0]) rotate([-90, 0, 0]) 
            cylinder(h=100, d=11.0); 
    }
    translate([0, -EPS, DZ_INSET/2]) rotate([-90, 0, 0]) 
        cylinder(h=100, d=4.4);
    translate([0, H - 1.5, DZ_INSET/2]) rotate([-90, 0, 0]) 
        cylinder(h=100, d=8.0);

    X0 = 6;
    Z0 = 3.5;
    translate([0, H - 1.5, DZ_INSET/2 - Z0]) segment(X0, 20);
    translate([0, H - 1.5, DZ_INSET/2 + Z0]) segment(X0, 20);
    translate([0, H - 1.5, DZ_INSET/2 - Z0]) segment(-X0, -20);
    translate([0, H - 1.5, DZ_INSET/2 + Z0]) segment(-X0, -20);

}
}
