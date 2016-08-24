include <buttress.scad>
use <battery.scad>

$fn = 90;
D_HOLE = 18;
Y_BIAS = 9.5;  //10;
X_BIAS = -1;
//H_MOUNT = 2;

translate([0, 0, H_BUTTRESS]) {
    //battery();
    //cylinder(h=H_BATTERY, d=D_BATTERY);
    //cylinder(h=H_BATTERY, d=D_AFT);
}

difference()
{
    part(rods=true, pcb=0);
    cylinder(h=H_BUTTRESS, d=D_BATTERY);
    
    translate([-DISPLAY_W/2 + X_BIAS, Y_BIAS, 0]) {
        cube(size=[DISPLAY_W, 20, H_BUTTRESS]);
    }
}

intersection()
{
    cylinder(h=H_BUTTRESS, d=D_INNER);
    for(i=[0:1]) {
        translate([DISPLAY_MOUNT_W/2 * ((i==0) ? -1 : 1), Y_BIAS, 0]) {
            rotate([-90, 0, 0]) {
                cylinder(h=20, d=D_DISPLAY_MOUNT);
            }
        }
    }
}