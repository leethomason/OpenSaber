include <dim.scad>
use <vents.scad>
use <../shapes.scad>
use <buttress.scad>

EPS = 0.1;
EPS2 = EPS * 2;
$fn = 90;

H_AFT = 30;

module battery()
{
    cylinder(h=H_BATTERY, d=D_BATTERY);
}


difference() {
    part(lowerWiring=true, upperWiring=true);
    
    // leave space for display
    translate([-20, 8, -EPS]) {
        cube(size=[40, 20, H_BUTTRESS + EPS2]);
    }
 
    // diameter of battery is 18
    cylinder(d=12, h=H_BUTTRESS);
}
