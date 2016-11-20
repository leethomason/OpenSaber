$fn = 180;
difference() {
    cylinder(h=14, d=(5.5 - 0.2));
    translate([0,0,6]) cylinder(h=8, d=3);
}
cylinder(h=5, d=9);