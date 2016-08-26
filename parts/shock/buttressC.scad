include <buttress.scad>

$fn = 90;
D_HOLE = 18;
Y_BIAS = 10;
//H_MOUNT = 2;

module displayButtress(useRods) {
    difference()
    {
        part(rods=useRods, pcb=4);
        cylinder(h=H_BUTTRESS, r=D_HOLE/2);
        
        translate([-DISPLAY_W/2, Y_BIAS, 0]) {
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
}

displayButtress(useRods);