include <dim.scad>
use <../shapes.scad>

$fn = 60;

Y_DOWN = 0.5;
H_BUTTON = 2.5;
H_PIN = 1.5;
H_SKIRT = 2.0;
W_SKIRT = 6.6;
D = 9.1;
R = 0.3;

module torus()
{
    rotate([90, 0, 0]) {
        rotate_extrude(convexity = 10)
            translate([D/2 - R, 0, 0])
                circle(r = R);
    }
}

module body()
{
    difference() {
        translate([0, 0, -20]) tube(h=40, do=40, di=D_INNER);
        rotate([-90, 0, 0]) cylinder(h=20, d=D);
    }
}

difference() {
    translate([0, D_INNER/2 - Y_DOWN - H_SKIRT, 0]) 
        rotate([-90, 0, 0]) 
            cylinder(h=H_BUTTON + H_SKIRT, d=12.0);
    body();
    // Pin
    translate([0, D_INNER/2 - Y_DOWN, 0]) 
        rotate([-90, 0, 0])
            cylinder(h=H_PIN, d=3.6);
    translate([-W_SKIRT/2, D_INNER/2 - Y_DOWN - H_SKIRT, -W_SKIRT/2]) 
        cube(size=[W_SKIRT, H_SKIRT, W_SKIRT]);
}

//torus();
