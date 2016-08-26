include <dim.scad>
use <vents.scad>
use <../shapes.scad>
use <buttress.scad>

EPS = 0.1;
EPS2 = EPS * 2;
$fn = 90;

H_AFT = 30;

difference() {
    union() {
        part(lowerWiring=true, upperWiring=false, showBolt=true);
        translate([0, 0, -H_AFT]) {
            cylinder(h=H_AFT, d=D_AFT);
        }
    }
    translate([0, 0, -H_AFT + H_BUTTRESS]) {
        cylinder(h=H_AFT + 20, d=D_BATTERY);
    }
    BACK = 8;
    UP = 9;
    translate([-20, -UP, -BACK]) {
        cube(size=[40, UP*2, BACK]);
    }
    
    // diameter of battery is 18
    //cylinder(d=12, h=H_BUTTRESS);
}
