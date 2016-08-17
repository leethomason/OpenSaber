include <dim.scad>

EPS = 0.1;
EPS2 = EPS * 2;
CRYSTAL_Y = 8;
ROD_X = 11;
ROD_Y = 3.5;
D_ROD = 3.4;

module part(battery=false, pcb=0, crystal=false, showBolt=false, rods=true, biasRight=false) {
    notch = true;

    difference() {
        cylinder(h=H_BUTTRESS, r=D_INNER/2, $fn=FACES);
        
        if (battery) {
            translate([0,.230 * INCHES_TO_MM, -EPSILON]) {
               cylinder(h=H_BUTTRESS + EPS2, r=D_BATTERY/2, $fn=FACES);    
            }
        }
        
        if(pcb > 0) {
            OFFSET = 40;

            translate([-W_PCB/2 + (biasRight ? 0 : -OFFSET), -11, -EPS]) {
                // Front: H_PCB
                // Back pins: H_PCB + 1
                // Center: 12
                cube(size=[W_PCB + OFFSET, pcb, H_BUTTRESS + EPS2]);
            }
        }

        if (crystal) {
            translate([0, CRYSTAL_Y, -EPS]) {
                cylinder(h=H_BUTTRESS + EPS2, r=D_CRYSTAL/2, $fn=FACES);

            }
            translate([0, CRYSTAL_Y/2, -EPS]) 
            { 
                //rotate([0, 0, 60]) {
                //    cube(size=[20, 20, H_BUTTRESS + EPS2]);
                //}
                scale([1, 2, 1]) {
                    rotate([0, 0, 45]) {
                        cube(size=[20, 20, H_BUTTRESS + EPS2]);
                    }
                }
            }
        }

        if (rods) {
            for(r=[0:1]) {
                translate([0, ROD_Y, -EPS]) {
                    rotate([0, 0, r*180]) {
                        translate([ROD_X, 0, 0]) {
                            cylinder(r=D_ROD/2, h=H_BUTTRESS + EPS2, $fn=FACES);
                        }
                    }
                }
            }
        }

        if (notch) {
            translate([-W_NOTCH/2, -D_INNER/2, -EPS]) {
                cube(size=[W_NOTCH, H_NOTCH, H_BUTTRESS + EPS2]);
            }
        }

    }
    // Rods.
    if (showBolt) {
        for(r=[0:1]) {
            translate([0, ROD_Y, 0]) {
                rotate([0, 0, r*180]) {
                    translate([ROD_X, 0, H_BUTTRESS]) {
                        color("red") {
                            cylinder(r=D_ROD/2, h=2);
                        }
                        color("green") {
                            cylinder(r=4.8, h=1);
                        }
                    }
                }
            }
        }
    }
}
