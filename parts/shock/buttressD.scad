use <../threads.scad>
include <dim.scad>
use <shapes.scad>

H_CUT = H_BUTTRESS + EPSILON*2;
H=0.7;

difference() {
    translate([0,0,-H/2]) {
        tube(H, D_WALL/2, D_WALL/2-0.08);
    }
    translate([0.42,0,0]) {
       rotate([0,90,0]) {
           scale(1/25.4) {
               english_thread(diameter=1/2, length=0.25, threads_per_inch=28, internal=true);
            }
        }
    }
}

