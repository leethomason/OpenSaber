ITOMM = 25.4;

D = 1.45 * ITOMM;
Y0 = 1.45 * ITOMM;
Y1 = 0.20 * ITOMM;
Y = Y0 + Y1;
X = 2.9 * ITOMM;
X1 = 1.1 * ITOMM;
X0 = (X - X1) / 2;
Z = 1.0 * ITOMM;

$fn = 80;

difference() {
    translate([0, -Y, 0]) cube(size=[X, Y, Z]);
    translate([0, -Y0/2, Z + D/2 - 7.0]) rotate([0, 90, 0]) cylinder(h=100, d=D);
}