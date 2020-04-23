
include <dim.scad>

//use <../commonUnified.scad>
use <../shapes.scad>
//use <../inset.scad>

START = M_PORT - 8.0;
END   = M_SWITCH + 8.0;

W = 16.0;
L = END - START;
M_PLATE = 0.0;
DY_PLATE = 1.8;

CUT_W = 16.0;
CUT_L = M_AFT_THREAD_FRONT - M_JOINT;

T = 25.4 / 4;
T0 = 3.0;

$fn = 80;

/*
module baseShape()
{
    difference() {
        translate([-W/2, D_OUTER/2 - DY_PLATE, START]) 
            roundedRect([W, T - T0, L], r=3.175/2, up="y");
        // Remove case.
        cylinder(h=300, d=D_OUTER);
    }
    // add connection
    intersection() {
        tube(h=300, do=D_OUTER, di=D_INNER);
        translate([-CUT_W/2, D_OUTER/2 - T0, M_JOINT])
            roundedRect([CUT_W, 100, CUT_L], r=3.175/2, up="y");
    }
}
*/

module baseShape()
{
    translate([-CUT_W/2, D_OUTER/2 - T0, START])
        roundedRect([CUT_W, T, END - START], r=3.175/2, up="y");
}

difference() {
    baseShape();

    translate([0, 0, M_PORT]) rotate([-90, 0, 0]) cylinder(h=100, d=11.0);
    translate([0, 0, M_BOLT]) rotate([-90, 0, 0]) cylinder(h=100, d=4.2);
    translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=100, d=8.0);
}

