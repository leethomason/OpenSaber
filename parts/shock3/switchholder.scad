include <dim.scad>

EPS = 0.01;
$fn = 40;

// Aligned to top
module cap() {
    color("gold") {
        translate([0, 0, -BUTTON_HEAD_H])
            cylinder(h=BUTTON_HEAD_H, d=BUTTON_HEAD_D);
        translate([0, 0, -BUTTON_HEAD_H - BUTTON_THREAD_H])
            cylinder(h=BUTTON_THREAD_H+EPS, d=BUTTON_THREAD_D- 0.1);
    }    
}

intersection() {
    //translate([-100, -100, -100]) cube(size=[200, 100, 200]);
    translate([-100, -100, -100]) cube(size=[200, 200, 200]);
    union() {
        difference() {
            union() {
                translate([0, 0, -BUTTON_WELL])
                    cylinder(h=BUTTON_WELL, d=BUTTON_HEAD_D);
                translate([0, 0, -BUTTON_WELL - BUTTON_UNDER_WELL])
                    cylinder(h=BUTTON_UNDER_WELL, d=BUTTON_UNDER_WELL_D);
            }
            translate([0, 0, -100]) {
                cylinder(h=100 - BUTTON_THREAD_H, d=TACTILE_POST_D);
                cylinder(h=200, d=BUTTON_THREAD_D);
            }
        }

        *translate([0, 0, BUTTON_HEAD_H]) cap();
    }
}
