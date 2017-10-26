$fn = 90;

H = 2.5;
D = 5.5;
OUTDER_D = 12;

difference() {
    hull() {
        cylinder(d=OUTDER_D, h=H);
        translate([40, 0, 0]) cylinder(d=OUTDER_D, h=H);
    }    
    cylinder(d=D, h=H);
    translate([40, 0, 0]) cylinder(d=D, h=H);
}
