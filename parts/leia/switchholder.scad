include <dim.scad>

EPS = 0.01;
$fn = 40;


BUTTON_WELL = 4.0;      // adjusted, depending. was: 3.5, 4.5
BUTTON_HEAD_D = 7.2;
BUTTON_HEAD_H = 1.6;
BUTTON_THREAD_H = 4.0;
BUTTON_THREAD_D = 4.0;
BUTTON_UNDER_WELL = 4.0;
BUTTON_UNDER_WELL_D = 10.0;

TACTILE_POST_H = 5.5;
TACTILE_POST_D = 3.4;
TACTILE_BASE_H = 3.8;
TACTILE_BASE_X = 6.0;

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
            *translate([BUTTON_HEAD_D/2, -50, -100])
                cube(size=[100, 100, 100]);
        }
        *translate([0, 0, BUTTON_HEAD_H]) cap();
    }
}
