include <dim.scad>
use <../shapes.scad>

$fn = 180;
H = 58.5;
CUT = 28;
D_OUTER = 37;    // a bit of padding

difference()
{
    union()
    {
        tube(h=H, di=D_OUTER, do=D_OUTER + 4);
        DX = 42;
        DY = 14;
        difference() {
            translate([-DX/2, -DY/2, 0]) 
                cube(size=[DX, DY, H]);
            cylinder(h=H, d=D_OUTER+3);
        }
        difference() {
            translate([-DY/2, -DX/2, 0]) 
                cube(size=[DY, DX, H]);
            cylinder(h=H, d=D_OUTER+3);
        }
    }
    hull() {
        translate([0, 0, H/2 - CUT/2 + 6])
            rotate([-90, 0, 0])
                cylinder(h=100, d=12);
        translate([0, 0, H/2 + CUT/2 - 6])
            rotate([-90, 0, 0])
                cylinder(h=100, d=12);
    }
    DOTSTAR = 7;
    NDOT = 6;
    LDOT = (NDOT - 1) * DOTSTAR;
    
    for(x=[0:NDOT-1]) {
        translate([0, 0, H/2 - LDOT/2 + x * DOTSTAR])
            rotate([90, 0, 0])
                cylinder(h=100, d=1);
    }
    
    translate([0, 0, H/2])
        rotate([0, 90, 0])
            cylinder(h=100, d=3);
}


