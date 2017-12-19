$fn = 60;

H = 2;
D = 5.5;
OUTER = 45;

difference() {
    hull() {
        translate([0,  0, 0]) cylinder(h=H, d=D);
        translate([OUTER+4, 0, 0]) cylinder(h=H, d=D);
        translate([0,  OUTER, 0]) cylinder(h=H, d=D);
        translate([OUTER+4, OUTER, 0]) cylinder(h=H, d=D);
    }
    translate([2,     2,    0]) cylinder(h=H, d=D);
    translate([OUTER, 2,    0]) cylinder(h=H, d=D);
    translate([2,     2+40, 0]) cylinder(h=H, d=D);
    translate([OUTER, 2+40, 0]) cylinder(h=H, d=D);
    
    for(x=[0:3]) {
        for(y=[0:2]) {
            translate([9+x*10, 10+y*12, 0]) cylinder(h=H, d=9);
        }
    }
}