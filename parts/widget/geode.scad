use <../shapes.scad>

$fn=40;

hull() {
    polygonXY(3, [[0,0], [3,16], [-3,16]]);
    rotate([0, 0, 120]) polygonXY(3, [[0,0], [3,16], [-3,16]]);
    rotate([0, 0, 240]) polygonXY(3, [[0,0], [3,16], [-3,16]]);
}

module pillar() {
    difference() {
        cylinder(h=17, d=5.8);
        translate([0, 0, 21]) 
            rotate([30, 0, 0]) cube(size=[10, 20, 10], center=true);
    }
}

translate([0, 12, 0]) pillar();
rotate([0, 0, 120]) translate([0, 12, 0]) pillar();
rotate([0, 0, 240]) translate([0, 12, 0]) pillar();

