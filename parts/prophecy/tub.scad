include <dim.scad>
$fn = 90;

D_OUTER = 37;
D_INNER = 32;
D = D_OUTER;

L = 84;
W = D_OUTER + 20;
T = 1;
H = D_OUTER + 20;


intersection() {
    
    translate([H/2, 0, 0]) cube(size=[H/2, 200, 200]);
    //cube(size=[H/2, 200, 200]);
    
    difference()
    {
        cube(size=[H + T, W + T*2, L + T*2]);
        translate([T, T, T]) {
            cube(size=[200, W, L]);
        }
        translate([H/2 + 2, W/2 + T, -1]) {
            cylinder(h=200, d=D);
        }
    }
}