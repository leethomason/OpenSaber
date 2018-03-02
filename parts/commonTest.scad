include <commonUnified.scad>

$fn = 40;

powerPortRing(30, 4, 22, 10);

translate([0, 0, 50]) switchRing(30, 3, 30, 15);

translate([0, 0, -40]) speakerHolder(24, 20, 4);