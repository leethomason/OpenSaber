use <../threads.scad>
include <dim.scad>
use <shapes.scad>

H_CUT = H_BUTTRESS + EPSILON*2;
H = 0.7 * INCHES_TO_MM;
T_WALL = 3;

difference() 
{
    translate([0,0,-H/2]) {
        tube(H, D_WALL/2, D_WALL/2 - T_WALL);
    }
  
    translate([D_WALL/2 - T_WALL - 2, 0, 0]) {
        rotate([0,90,0]) {
            english_thread(diameter=1/2, length=T_WALL + 3, threads_per_inch=28, internal=true);
            //cylinder(d=12.5, h=T_WALL + 4);
        }
    }
}

