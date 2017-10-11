include <dim.scad>

$fn = 80;

R_SABER_OUTER = D_SABER_OUTER / 2;
EPS = 0.001;

RF24_Z = 29;
RF24_Y = 3.2;
RF24_X = 16;

RF24_CRYSTAL_Z  = 4.5;
RF24_CRYSTAL_DZ = 12.5;
RF24_CRYSTAL_DX = 5;
RF24_DECO       = 5;

RF24_ANT_Z      = 20;
RF24_ANT_DZ     = RF24_Z - RF24_ANT_Z;

WALL_T          = 1;

BOLT_HEAD_D     = 5.5;                
BOLT_D          = 3.2;   
BOLT_INSET      = 2.5;
BOLT_Z0         = 4;
BOLT_Z1         = BOLT_Z0 + 35;   // only specific intervals work
                                    // due to grip spacing
BOLT_DZ = BOLT_Z1 - BOLT_Z0;

BOX_DZ = BOLT_Z0 + BOLT_Z1;
BOX_DY = 5;
BOX_DX = RF24_X + WALL_T * 2;

RF24_START_Z = (BOX_DZ - RF24_Z)/2;

RAD = 1;

module bolt(boltD, boltHeadD, boltHeadH) {
   cylinder(d=boltD, h = 100);
   cylinder(d=boltHeadD, h=boltHeadH);
}

module roundCube(size, rad)
{
    hull() {
        translate([rad, 0, rad]) 
            rotate([-90, 0, 0])
                cylinder(h=size[1], r=rad);
        translate([size[0]-rad, 0, rad]) 
            rotate([-90, 0, 0])
                cylinder(h=size[1], r=rad);
        translate([rad, 0, size[2]-rad]) 
            rotate([-90, 0, 0])
                cylinder(h=size[1], r=rad);
        translate([size[0]-rad, 0, size[2]-rad]) 
            rotate([-90, 0, 0])
                cylinder(h=size[1], r=rad);
    }
}

difference() {
    translate([-BOX_DX/2, 0, 0]) {
        roundCube(size=[BOX_DX,
                        R_SABER_OUTER + BOX_DY,
                        BOX_DZ],
                  rad=RAD);
    }
    
    // Punch out the actual chip
    translate([-RF24_X/2, 0, RF24_START_Z])
        cube(size=[RF24_X, R_SABER_OUTER + RF24_Y, RF24_Z-8]);
    
    // Crystal opening
    translate([-RF24_X/2, 0, RF24_START_Z + RF24_CRYSTAL_Z])
        roundCube(size=[RF24_CRYSTAL_DX, 100, RF24_CRYSTAL_DZ], rad=RAD);
    
    translate([RF24_X/2 - RF24_DECO, 0, RF24_START_Z + RF24_CRYSTAL_Z])
        roundCube(size=[RF24_DECO, 100, RF24_DECO*1.5], rad=RAD);
    
    // Antenna
    translate([-RF24_X/2, 0, RF24_START_Z + RF24_ANT_Z])
        cube(size=[RF24_X, R_SABER_OUTER + RF24_Y-1.5, RF24_ANT_DZ]);
    
    translate([-RF24_X/2+1, 0, RF24_START_Z + RF24_ANT_Z])
        roundCube(size=[RF24_X-2, 100, RF24_ANT_DZ], rad=RAD);
    

    // Bolts
    translate([0, D_SABER_OUTER / 2 + BOX_DY + EPS, BOLT_Z0]) rotate([90, 0, 0])
        bolt(boltD=BOLT_D, boltHeadD=BOLT_HEAD_D, boltHeadH=BOLT_INSET);
    
    translate([0, D_SABER_OUTER / 2 + BOX_DY + EPS, BOLT_Z1]) rotate([90, 0, 0])
        bolt(boltD=BOLT_D, boltHeadD=BOLT_HEAD_D, boltHeadH=BOLT_INSET);
    
    // Saber body
    cylinder(d=D_SABER_OUTER, h=100);
}