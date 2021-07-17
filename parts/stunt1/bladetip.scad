//INNER = 0.88 * 25.4;
INNER = 0.75 * 25.4;
OUTER = 1.00 * 25.4;
H = 6.0;

$fn = 40;

cylinder(h=H, d=INNER);
translate([0, 0, H]) {
    intersection() {
        translate([-50, -50, 0]) cube(size=100, 100, 100);
        sphere(d=OUTER);
    }
}