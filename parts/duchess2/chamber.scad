use <../commonUnified.scad>
use <../shapes.scad>
include <dim.scad>

DZ_PART = 63.5;
DZ_THREAD = 10.8;
DZ_SECTION = DZ_PART - DZ_THREAD * 2;
DZ_WINDOW = 20.8;
DZ_OPENING = 5.0;

D_INNER = 25.8;
D_OUTER = 36.9;
D_THREAD = 34.2;

EPS = 0.01;
$fn = 40;

module case() {
    color("silver") difference() {
        union() {
            cylinder(h=DZ_PART, d=D_THREAD);
            translate([0, 0, DZ_THREAD]) cylinder(h=DZ_SECTION, d=D_OUTER);
        }
        translate([0, 0, -EPS]) cylinder(h=100, d=D_INNER);
        translate([0, 0, DZ_WINDOW + DZ_OPENING/2]) capsule(-20, 20, DZ_OPENING/2, true);
        translate([0, 0, DZ_WINDOW + DZ_OPENING/2]) capsule(70, 110, DZ_OPENING/2, true);
        translate([0, 0, DZ_PART - DZ_WINDOW - DZ_OPENING/2]) capsule(-20, 20, DZ_OPENING/2, true);
        translate([0, 0, DZ_PART - DZ_WINDOW - DZ_OPENING/2]) capsule(70, 110, DZ_OPENING/2, true);
    }
}

module rods() {
    color("PaleGoldenrod") translate([0, D_INNER/2 - D_ROD_NUT/2]) cylinder(h=DZ_PART, d=D_ROD);
    color("gold") rotate([0, 0, ANGLE_OFFSET]) translate([0, D_INNER/2 - D_TUBE/2]) cylinder(h=DZ_PART, d=D_TUBE);
}

module crystal() {
    color("Aquamarine") polygonXY(h=DZ_SECTION, points=[
        [-3, -3], [3, -3], [4, 0], [3, 3], [-3, 3], [-4, 0]
    ]);
}

module ring() {
    color("gold") translate([0, 0, -CHAMBER_RING_DZ/2]) difference() {
        cylinder(h=CHAMBER_RING_DZ, d=D_INNER);
        cylinder(h=CHAMBER_RING_DZ, d=D_CRYSTAL_SPACE);
    }
}

*case();
rotate([0, 0, A_BOLT_0]) rods();
rotate([0, 0, A_BOLT_1]) rods();
translate([0, 0, DZ_THREAD]) crystal();
//translate([0, 0, DZ_THREAD]) ring();
translate([0, 0, DZ_PART/2]) ring();
translate([0, 0, DZ_PART - DZ_THREAD]) ring();


echo("angle: rod0", A_BOLT_0, "tube0", A_BOLT_0 + ANGLE_OFFSET);
echo("r: rod0", D_INNER/2 - D_ROD_NUT/2, "tube0", D_INNER/2 - D_TUBE/2);