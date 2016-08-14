include <dim.scad>

EPS = 0.1;
EPS2 = EPS * 2;
CRYSTAL_Y = 8;

module part(battery=false, pcb=false, crystal=false) {
    difference() {
        cylinder(h=H_BUTTRESS, r=D_INNER/2, $fn=FACES);
        
        if (battery) {
            translate([0,.230 * INCHES_TO_MM, -EPSILON]) {
               cylinder(h=H_BUTTRESS + EPS2, r=D_BATTERY/2, $fn=FACES);    
            }
        }
        
        if(pcb) {
            translate([-W_PCB/2, -0.41 * INCHES_TO_MM, -EPS]) {
              cube(size=[W_PCB + 40, H_PCB, H_BUTTRESS + EPS2]);
            }
        }

        if (crystal) {
            translate([0, CRYSTAL_Y, -EPS]) {
                cylinder(h=H_BUTTRESS + EPS2, r=D_CRYSTAL/2, $fn=FACES);

            }
            translate([0, CRYSTAL_Y/2, -EPS]) 
//            translate([0, 0, -EPS]) 
            { 
                rotate([0, 0, 60]) {
                    cube(size=[20, 20, H_BUTTRESS + EPS2]);
                }
            }
        }

        /*
        // Rods.
        translate([-D_INNER/2 + D_ROD/2 + ROD_GAP, 0, 0]) {
            cylinder(h=H_CUT, r=D_ROD/2, $fn=FACES/2);
        }
        translate([D_INNER/2 - D_ROD/2 - ROD_GAP, 0, 0]) {
            cylinder(h=H_CUT, r=D_ROD/2, $fn=FACES/2);
        }
        */
    }
}