CX = 7.63;
CZ = 33.5;
DY = 3.4;
DIAMETER = 38.0;

$fn = 100;

difference() {
    translate([-CX/2, 0, 0]) cube(size=[CX, 10, CZ]);
    translate([0, DY + DIAMETER/2, 0]) cylinder(h=100, d=DIAMETER);

    // 6.2, d=4.2
    translate([0, 0, 6.2]) rotate([-90, 0, 0]) cylinder(h=10, d=4.4);
    // 12.7, d=3.2
    translate([0, 0, 12.7]) rotate([-90, 0, 0]) cylinder(h=10, d=3.5);
    // 25.3, d=3.2
    translate([0, 0, 25.3]) rotate([-90, 0, 0]) cylinder(h=10, d=3.5);
}
