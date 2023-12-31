DX1 = 51.1;
DX0 = 28.3 - 0.5;
DY1 = 17.7;
DY0 = 6.0;
DZ0 = 10.0;

D_TUBE = 32.0;

//$fn = 80; // overrides fs; num fragments
$fa = 6;   // minimum angle (default 12)
$fs = 0.8;  // minimum size (default 2)

difference() {
    union() {
        translate([-DX0/2, 0, 0]) cube(size=[DX0, DY0, DZ0]);
        translate([-DX1/2, DY0, 0]) cube(size=[DX1, DY1, DZ0]);
    }
    translate([0, DY0 - 1, 20.0]) rotate([-90, 0, 0]) cylinder(r=D_TUBE/2, h=100);
}