include <dim.scad>

$fn = 90;

H = 50;
D = 13;

module innerTube()
{
    translate([0, 0, M_WAY_BACK]) {
        cylinder(d=D_AFT, h=M_TRANSITION - M_WAY_BACK);
    }
    translate([0, 0, M_TRANSITION]) {
        cylinder(d=D_FORWARD, h = H_FAR);
    }
}

intersection() {
    innerTube();
    translate([0, -20, 120]) rotate([-90, 0, 0]) cylinder(h=H, d=D);
}

intersection() {
    cylinder(h=200, d=D_SABER_OUTER);
    translate([0, 0, 120]) rotate([-90, 0, 0]) cylinder(h=H, d=D);
}

