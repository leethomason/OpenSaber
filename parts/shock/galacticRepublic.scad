use <../shapes.scad>

$fa = 1;
$fs = 1;

//SCALE = 25.4 / 100;
SCALE = 18 / 100;

scale([SCALE, SCALE, SCALE]) {
    difference () {
        cylinder(h=10, d=100);
        translate([0,0,5]){
            color("red") {
                cylinder(h=6, d=90);
            }
        }
        
    }

    difference () {
        cylinder(h=10, d=85);
        translate([0,0,5]){
            color("red") {
                cylinder(h=6, d=75);
            }
        }
        color ("blue") {
            gap();
            rotate(90) {
                gap();
            }
            rotate(45) {
                gap();
            }
            rotate(-45) {
                gap();
            }
        }
    }

    union() {
        cylinder(h=10, d=30);
        color ("green") {
            block();
            rotate(90) {
                block();
            }
            rotate(45) {
                block();
            }
            rotate(-45) {
                block();
            }
        }
    }
}

*translate([0, 0, -1]) {
    difference() {
        cylinder(h=5, d=26);
        cylinder(h=5, d=25.4 * (1 - 1/8));
    }
}

translate([0, 0, -2]) {
    cylinder(h=2, d=25.4);
}

module gap() {
    translate([0,0,8]) {
            cube([150,12,6], center=true);
    }
}
module block() {
    translate([0,0,7]) {
            cube([65,6,6], center=true);
    }
}
