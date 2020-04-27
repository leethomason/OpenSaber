
include <dim.scad>

//use <../commonUnified.scad>
use <../shapes.scad>
//use <../inset.scad>

START = max(M_PORT - 8.0, M_JOINT + 1.0);
END   = min(M_SWITCH + 7.0, M_AFT_THREAD_FRONT);    // aft thread front has padding

echo("Plate cut, start", START, "end", END, "width", DX_SWITCH_PLATE);

W = 16.0;
L = END - START;
M_PLATE = 0.0;
DY_PLATE = 1.8;

CUT_L = M_AFT_THREAD_FRONT - M_JOINT;

T = 25.4 / 4;
T0 = 3.0;
SINK = 1.0;

$fn = 80;

if (START < (M_JOINT - 1.0)) {
    echo("ERROR switchplate start can't push on joint (battery)");
}

TOP = D_OUTER/2 - T0 + T;

module baseShape()
{
    translate([-DX_SWITCH_PLATE/2, D_OUTER/2 - T0, START])
        roundedRect([DX_SWITCH_PLATE, T, END - START], r=3.175/2, up="y");
}

difference() {
    baseShape();

    translate([0, 0, M_PORT]) rotate([-90, 0, 0]) cylinder(h=100, d=11.0);
    translate([0, 0, M_BOLT]) rotate([-90, 0, 0]) cylinder(h=100, d=4.2);
    translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=100, d=8.0);

    translate([0, TOP - SINK, M_BOLT]) rotate([-90, 0, 0]) cylinder(h=100, d=8.0);
    hull() {
        translate([0, TOP - SINK, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=100, d=11.0);
        translate([0, TOP - SINK, M_SWITCH + 20]) rotate([-90, 0, 0]) cylinder(h=100, d=11.0);
    }
}

