$fn = 90;
D_CENTER_OUTER = 36;

DZ = 75;
PLATE_DX = 10;
PLATE_DZ = 60;
BOX_DX = 14;
BOX_DZ = 75;

cylinder(h=DZ, d=D_CENTER_OUTER);

difference() {
    translate([-BOX_DX/2, 0, 0]) cube(size=[BOX_DX, 20, DZ]);
    translate([-PLATE_DX/2, 0, (DZ - PLATE_DZ)/2]) cube(size=[PLATE_DX, 20, PLATE_DZ]);
}