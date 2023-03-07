/* Here to help test & set up new printer(s) 
*/

$fn = 80;

S = 10;

cube(size=[S, S, S/2]);

intersection() {
    rotate([-90, 0, 0]) translate([S/2, 0, 0]) scale([0.6, 1, 1]) cylinder(h=20, d=S*3);
    difference() {
        translate([0, 0, S/2]) cube(size=[S, S/4, S]);
        translate([0, 10, S/2]) rotate([90, 0, 0]) linear_extrude(20)
            polygon([[S/4, 0], [S/4, S/2], [S/2, S-1], [3*S/4, S/2], [3*S/4, 0]]);
    }
}

translate([S, 0, S]) cube(size=[S * 0.6, S/2, S/4]);

translate([S/2, S * 0.75, 0]) {
    difference() {
        cylinder(h=S*1.5, d=S * 0.4);
        translate([0, 0, S*1.5 - 3.0])
            cylinder(h=10, d=2.0);
    }
}