$fn = 40;

H = 6.0;

difference() {
    cylinder(h=H, d=8.5);
    translate([0, 0, -0.001]) cylinder(h=4.0, d=5.6);
}