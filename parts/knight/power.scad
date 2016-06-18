use <../threads.scad>
use <../shapes.scad>
include <dim.scad>

intersection() {
    difference() {
        cylinder(h=PORT_LEN, r=R_AFT, $fn=FACES);
        translate([0,0,PORT_SCREW_PAD + R_PORT_SCREW]) {
            rotate([0,90,0]) {
                if (false) {
                    english_thread(diameter=0.5, length=PORT_LEN * MM_TO_INCHES, threads_per_inch=28, internal=true);
                }
                else {
                    cylinder(h=PORT_LEN, r=D_PORT_SCREW/2, $fn=FACES);
                }
            }
        }
    }
    
    translate([R_FORWARD - H_PORT,-100,0]) {
        cube(size=[200, 200, 200]);
    }
}
