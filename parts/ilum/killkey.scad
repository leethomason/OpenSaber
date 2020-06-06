$fn = 180;

H = 6.0;
difference() {
    cylinder(h=H, d=10.8);
    translate([0, 0, H-3.0]) cylinder(h=10, d=9.4);
    cylinder(h=10, d=5.45);
}
