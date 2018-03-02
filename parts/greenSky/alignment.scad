$fn = 90;

difference() {
    union() {
        cylinder(h=10, d=15);
        difference() {
            translate([0,0,6]) cylinder(h=4, d=22);
            translate([0,-20,-11]) rotate([-90,0,0]) cylinder(h=100, d=40);
        }
    }   
    cylinder(h=10, d=3.3);
}
