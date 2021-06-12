SPACE = 1.0;

/* Initiate
D0 = 35.3 + SPACE;
D1 = 31.4 + SPACE;
LENGTH = 48.0;
*/
D0 = 37.4;
D1 = 37.4;
LENGTH = 48.0;

WIDTH = 48.0;
HALF = (D0 > D1 ? D0 : D1) / 2 + 10;

T = 4.0;

$fn = 40;

difference()
{
    translate([-LENGTH/2 - T, -WIDTH/2 - T, 0]) cube(size=[LENGTH + T*2, WIDTH + T*2, HALF + T]);
    translate([-LENGTH/2, -WIDTH/2, T]) cube(size=[LENGTH, WIDTH, 100.0]);

    translate([0, 0, HALF + T]) rotate([0, 90, 0]) cylinder(h=1000, d=D0);
    translate([0, 0, HALF + T]) rotate([0, -90, 0]) cylinder(h=1000, d=D1);
}

translate([0, WIDTH + T*2 + 1, 0])
{
    difference()
    {
        translate([-LENGTH/2 - T, -WIDTH/2 - T, 0]) cube(size=[LENGTH + T*2, WIDTH + T*2, HALF]);
        translate([-LENGTH/2, -WIDTH/2, 0]) cube(size=[LENGTH, WIDTH, 100.0]);

        translate([0, 0, HALF]) rotate([0, 90, 0]) cylinder(h=1000, d=D0);
        translate([0, 0, HALF]) rotate([0, -90, 0]) cylinder(h=1000, d=D1);
    }
}