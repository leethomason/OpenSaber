include <dim.scad>
use <../shapes.scad>
use <util.scad>

$fn = 90;


module midSection() {
    difference() {
        tube(h=DZ_MID_OUTER, di=D_MID, do=D_MID_OUTER);

        SPACE = 2.0;
        END = DZ_MID_OUTER / 2 + PLATE_MOUNT_WITH_THREAD / 2;
        TOOL = 3.2;
        DZSLOT = END - SPACE - TOOL;
        HT = TOOL / 2;

        hull() {
            translate([ 5, 0, SPACE + HT]) rotate([-90, 0, 0]) cylinder(h=200, d=TOOL);
            translate([-5, 0, SPACE + HT]) rotate([-90, 0, 0]) cylinder(h=200, d=TOOL);
            translate([ 5, 0, SPACE + DZSLOT]) rotate([-90, 0, 0]) cylinder(h=200, d=TOOL);
            translate([-5, 0, SPACE + DZSLOT]) rotate([-90, 0, 0]) cylinder(h=200, d=TOOL);
        }
    }
}

midSection();

color("red") {
    DX_BUBBLE = 8; // without leads
    DY_BUBBLE = 5;  // more space for leads
    DZ_BUBBLE = 16;

    translate([-DX_BUBBLE/2, D_MID_OUTER/2 - 2, 20])
        cube(size=[DX_BUBBLE, DY_BUBBLE, DZ_BUBBLE]);
}

color("green") {
    translate([0, 0, 15]) rotate([-90, 0, 0]) cylinder(h=20, d=8);
}

color("gray") {
    S = 8.0;
    translate([-S/2, 0, 2.5]) cube(size=[S, 20, S]);
}

/*
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
*/

//inner();