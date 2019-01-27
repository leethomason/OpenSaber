use <../shapes.scad>
$fn = 100;

D_INNER = 25.4 * 0.92;
D_OUTER = D_INNER + 6.5;
DZ = 17;
BASE_X = 15;
BASE_Y = 8;
H_CUT = 14;

// base
difference()
{ 
    union() {
        translate([-BASE_X/2, 0, 0])
            cube(size=[BASE_X, BASE_Y, DZ]); 
        translate([0, 7 + D_INNER/2]) 
            tube(h=DZ, do=D_OUTER, di=D_INNER);        
    }
    CUT_X = 8;
    translate([-CUT_X/2, 4, 0])
        cube(size=[CUT_X, 10, DZ]);
    translate([-H_CUT/2, 10, -1])
        cube(size=[H_CUT, 50, 20]);

    translate([0, 0, DZ/2]) rotate([-90, 0, 0])
        cylinder(h=10, d=4);
}

translate([ 9, 30, 0])
    cylinder(h=DZ, d=5.5);
translate([-9, 30, 0])
    cylinder(h=DZ, d=5.5);
