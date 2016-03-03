include <dim.scad>

H_CUT = H_BUTTRESS + EPSILON*2;

SWITCHES = false;
BATTERY = true;
PCB = true;
DISPLAY = false;

difference() {
    cylinder(h=H_BUTTRESS, r=D_WALL/2, $fn=FACES);
    
    if (BATTERY) {
        //translate([0,-0.21,-EPSILON]) {       // battery in bottom
        //translate([0,-0.12,-EPSILON]) {         // fits battery in pommel
                //translate([0,0,-EPSILON]){              // battery center
        translate([0,.248,-EPSILON]) {
           cylinder(h=H_CUT, r=D_BATTERY/2, $fn=FACES);    
        }
    }
    
    if(PCB) {
        translate([0,-0.41,0]) {
          cube(size=[W_PCB,H_PCB,H_CUT], center=true);
        }
    }
    
    if (DISPLAY) {
//        translate([0,.3,0]) {
        translate([0,.4,0]) {
            cube(size=[W_OUTER_DISPLAY, H_OUTER_DISPLAY, H_CUT], center=true);
            translate([0,H_OUTER_DISPLAY,0]) {
                cube(size=[W_INNER_DISPLAY, H_INNER_DISPLAY, H_CUT], 
                     center=true);
            }
        }
    }
    
    if (SWITCHES) {
        translate([-D_SWITCH_CUTOUT/2, D_WALL/2 - H_SWITCH_CUTOUT, -EPSILON ]) {
            cube(size=[D_SWITCH_CUTOUT, H_SWITCH_CUTOUT, H_CUT], center=false);
        }
        translate([-D_POWER_CUTOUT/2, D_WALL/2 - H_POWER_CUTOUT, -EPSILON ]) {
            cube(size=[D_POWER_CUTOUT, H_POWER_CUTOUT, H_CUT], center=false);
        }
        
    //    translate([-D_DATA_CUTOUT/2, D_WALL/2 - H_DATA_CUTOUT, -EPSILON ]) {
    //        cube(size=[D_DATA_CUTOUT, H_DATA_CUTOUT, H_CUT], center=false);
    //    }
    }
}