use <../threads.scad>
use <../shapes.scad>
include <dim.scad>

H_BARREL_UPPER  = 2;
H_BARREL_BODY   = 3;
H_BARREL_LOWER  = 1;

D_THREAD        = 13;
T_WALL          = 2;
T_LIP           = 2;
T_OUTER_LIP     = 2;
T_LOWER_SPACE   = 1.5;

H_EXTENSION     = 0.0;
D_BARREL_UPPER  = D_THREAD - T_WALL - T_LIP - GAP * 2;
D_BARREL_BODY   = D_THREAD - T_WALL - GAP * 2;
D_BARREL_LOWER  = D_THREAD - T_WALL - T_LOWER_SPACE - GAP * 2;

// Inner barrel.
*union() {
    cylinder(h=H_BARREL_LOWER, r1=D_BARREL_LOWER/2, r2=D_BARREL_BODY/2, $fn=128);
    translate([0,0,H_BARREL_LOWER]) {
        cylinder(h=H_BARREL_BODY, r=D_BARREL_BODY/2, $fn=128);
        translate([0,0,H_BARREL_BODY]) {
            cylinder(h=H_BARREL_UPPER + H_EXTENSION, r=D_BARREL_UPPER/2, $fn=128);
        }
    }
}

// Outer holder.
color("green") {
    tube(H_BARREL_LOWER + H_BARREL_BODY, D_THREAD / 2, (D_THREAD - T_WALL)/2);
    translate([0,0,H_BARREL_LOWER + H_BARREL_BODY]) {
        tube(H_BARREL_UPPER, (T_OUTER_LIP + D_THREAD) / 2, (D_THREAD - T_WALL - T_LIP)/2);
    }
}