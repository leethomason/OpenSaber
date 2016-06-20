use <../threads.scad>
use <../shapes.scad>
include <dim.scad>

EPS = 0.1;
OFFSET = R_AFT - H_PORT;

difference() {
    intersection() {
        cylinder(h=PORT_LEN, r=R_AFT, $fn=FACES);
        translate([OFFSET,-100,0]) {
            cube(size=[200, 200, 200]);
        }
    }

    translate([OFFSET-EPS, 0, PORT_SCREW_PAD + R_PORT_SCREW]) {
        rotate([0,90,0]) {
            BOLT_LEN = H_PORT + EPS*2;
            if (true) {
                english_thread(diameter=0.5, length=BOLT_LEN * MM_TO_INCHES, threads_per_inch=28, internal=true);
            }
            else {
                cylinder(h=BOLT_LEN, r=D_PORT_SCREW/2, $fn=FACES);
            }
        }
    }
    
}

