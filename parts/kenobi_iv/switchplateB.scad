include <dim.scad>
use <util.scad>

$fn = 90;


difference() {
    union() {
        // Box
        difference() {
            translate([-BOX_DX/2, 0, 0]) 
                cube(size=[BOX_DX, D_MID_OUTER/2 + BOX_Y, DZ_MID_OUTER]);
        translate([-PLATE_DX/2, 0, (DZ_MID_OUTER - PLATE_DZ)/2]) 
            cube(size=[PLATE_DX, 100, PLATE_DZ]);            
        }

        // Switch mount
        DZ_PORT_MOUNT = 12;
        translate([-BOX_DX/2, 0, (DZ_MID_OUTER - DZ_PORT_MOUNT)/2]) 
            cube(size=[BOX_DX, D_MID_OUTER/2+3, DZ_PORT_MOUNT]);

        
    }

    cylinder(h=DZ_MID_OUTER, d=D_MID_OUTER);
    translate([0, 0, DZ_MID_OUTER/2]) rotate([-90, 0, 0]) 
        cylinder(h=100, d=D_POWER_PORT);
}

color("yellow") {
    // Thread zone
    translate([-PLATE_DX/2, D_MID_OUTER/2, (DZ_MID_OUTER - PLATE_MOUNT_WITH_THREAD)/2])
        cube(size=[PLATE_DX, 1, PLATE_MOUNT_WITH_THREAD]);
}

color("red") {
    DX_BUBBLE = 10; // maybe 8 or 9
    DY_BUBBLE = 5;  // more space for leads
    DZ_BUBBLE = 16;

    translate([-DX_BUBBLE/2, D_MID_OUTER/2 + 1, 31.5])
        cube(size=[DX_BUBBLE, DY_BUBBLE, DZ_BUBBLE]);
}


//inner();