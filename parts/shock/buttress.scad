include <dim.scad>

H_CUT = H_BUTTRESS + EPSILON*2;

module part(BATTERY, PCB, DISPLAY, SWITCHES) {
    difference() {
        cylinder(h=H_BUTTRESS, r=D_INNER/2, $fn=FACES);
        
        if (BATTERY) {
            //translate([0,-0.21,-EPSILON]) {       // battery in bottom
            //translate([0,-0.12,-EPSILON]) {         // fits battery in pommel
                    //translate([0,0,-EPSILON]){              // battery center
            translate([0,.230 * INCHES_TO_MM, -EPSILON]) {
               cylinder(h=H_CUT, r=D_BATTERY/2, $fn=FACES);    
            }
        }
        
        if(PCB) {
            translate([-W_PCB/2, -0.41 * INCHES_TO_MM, 0]) {
              cube(size=[W_PCB, H_PCB, H_CUT]);
            }
        }
        
        if (DISPLAY) {
            translate([0,0.4 * INCHES_TO_MM,0]) {
                // Punch out the top:
                //cube(size=[W_OUTER_DISPLAY, 1.0 * INCHES_TO_MM, H_CUT], center=true);

                //cube(size=[W_OUTER_DISPLAY, H_OUTER_DISPLAY, H_CUT], center=true);
                translate([-W_OUTER_DISPLAY/2, H_OUTER_DISPLAY, -EPSILON]) {
                    cube(size=[W_OUTER_DISPLAY, H_INNER_DISPLAY*2, H_CUT]);
                }
            }
        }
        
        if (SWITCHES) {
            translate([-D_SWITCH_CUTOUT/2, D_INNER/2 - H_SWITCH_CUTOUT, -EPSILON ]) {
                cube(size=[D_SWITCH_CUTOUT, H_SWITCH_CUTOUT, H_CUT], center=false);
            }
            translate([-D_POWER_CUTOUT/2, D_INNER/2 - H_POWER_CUTOUT, -EPSILON ]) {
                cube(size=[D_POWER_CUTOUT, H_POWER_CUTOUT, H_CUT], center=false);
            }
            
            //    translate([-D_DATA_CUTOUT/2, D_INNER/2 - H_DATA_CUTOUT, -EPSILON ]) {
            //        cube(size=[D_DATA_CUTOUT, H_DATA_CUTOUT, H_CUT], center=false);
            //    }
        }

        // Rods.
        translate([-D_INNER/2 + D_ROD/2 + ROD_GAP, 0, 0]) {
            cylinder(h=H_CUT, r=D_ROD/2, $fn=FACES/2);
        }
        translate([D_INNER/2 - D_ROD/2 - ROD_GAP, 0, 0]) {
            cylinder(h=H_CUT, r=D_ROD/2, $fn=FACES/2);
        }
    }
}