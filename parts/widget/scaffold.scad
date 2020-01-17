use <../shapes.scad>

INCH_TO_MM = 25.4;

PCB_W = 0.7 * INCH_TO_MM;
PCB_L = 1.4 * INCH_TO_MM;

SCAFFOLD_W = 1.1 * INCH_TO_MM;
SCAFFOLD_L = 1.8 * INCH_TO_MM;
SCAFFOLD_MOUNT_W = 0.9 * INCH_TO_MM;
SCAFFOLD_MOUNT_L = 1.6 * INCH_TO_MM;

SCAFFOLD_H = 36.0;
ACCESS_H = 20;

SCAFFOLD_SLOT_Z = 24.0;
SCAFFOLD_SLOT_W = 26.0;
SCAFFOLD_SLOT_H = 7.0;

HOLE_D = 1.7;

difference() {
    translate([-SCAFFOLD_W/2, -SCAFFOLD_L/2, 0]) {
        //cube();
        roundedRect(size=[SCAFFOLD_W, SCAFFOLD_L, SCAFFOLD_H], r=1.0, $fn=20);
    }

    // Cut out access
    translate([-PCB_W/2, -PCB_L/2, -1]) cube(size=[PCB_W, PCB_L, 200]);
    translate([-PCB_W/2, -100, -1]) cube(size=[PCB_W, 200, ACCESS_H+1]);
    translate([-100, -PCB_L/2, -1]) cube(size=[200, PCB_L, ACCESS_H+1]);

    // Cut out holder slot
    translate([-100, -SCAFFOLD_SLOT_W/2, SCAFFOLD_SLOT_Z]) cube(size=[200, SCAFFOLD_SLOT_W, SCAFFOLD_SLOT_H]);

    // Cut out bottom holes to affix to test board
    translate([SCAFFOLD_MOUNT_W/2, SCAFFOLD_MOUNT_L/2, -0.1]) cylinder(d=HOLE_D, h=10, $fn=20);
    translate([SCAFFOLD_MOUNT_W/2, -SCAFFOLD_MOUNT_L/2, -0.1]) cylinder(d=HOLE_D, h=10, $fn=20);
    translate([-SCAFFOLD_MOUNT_W/2, SCAFFOLD_MOUNT_L/2, -0.1]) cylinder(d=HOLE_D, h=10, $fn=20);
    translate([-SCAFFOLD_MOUNT_W/2, -SCAFFOLD_MOUNT_L/2, -0.1]) cylinder(d=HOLE_D, h=10, $fn=20);
}


