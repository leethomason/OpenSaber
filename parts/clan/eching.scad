D0 = 31.8;
LENGTH = 60.0;
WIDTH = 50.0;
HALF = 34.0;

T = 6.0;

$fn = 40;

difference()
{
    translate([-LENGTH/2 - T, -WIDTH/2 - T, 0]) cube(size=[LENGTH + T*2, WIDTH + T*2, HALF + T]);
    translate([-LENGTH/2, -WIDTH/2, T]) cube(size=[LENGTH, WIDTH, 100.0]);
    translate([-500, 0, HALF + T]) rotate([0, 90, 0]) cylinder(h=1000, d=D0);
}

translate([0, 65, 0])
{
    difference()
    {
        translate([-LENGTH/2 - T, -WIDTH/2 - T, 0]) cube(size=[LENGTH + T*2, WIDTH + T*2, HALF]);
        translate([-LENGTH/2, -WIDTH/2, 0]) cube(size=[LENGTH, WIDTH, 100.0]);
        translate([-500, 0, HALF]) rotate([0, 90, 0]) cylinder(h=1000, d=D0);
    }
}