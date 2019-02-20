include <dim.scad>
include <../shapes.scad>
include <../commonUnified.scad>

$fn=90;
EPS = 0.01;
EPS2 = EPS * 2;

H = 100;

Z_MID = 50;
Z0 = Z_MID - DZ_CENTER;
Z1 = Z_MID + DZ_CENTER;

// Retaining bolt.
BOLT_D = 4.5;
NUT_W = 8.6;
NUT_Y = 3.4;

SWITCH_BRIDGE_DZ = 5.8;

MID_BRIDGE_DZ = 10;

module case(extend = 0)
{
    color("silver") difference() 
    {
        tube(h=H, do=D_OUTER + extend, di=D_INNER);
        hull() {
            translate([0, 0, Z_MID - DZ_CENTER]) rotate([-90, 0, 0]) cylinder(h=100, d=D_CAPSULE);
            translate([0, 0, Z_MID + DZ_CENTER]) rotate([-90, 0, 0]) cylinder(h=100, d=D_CAPSULE);
        }
    }
}


module capsule(extends, dy, deltaD=0)
{
    hull() {
        translate([0, D_OUTER/2 + dy , Z0]) rotate([-90, 0, 0]) cylinder(h=extends, d=D_CAPSULE + deltaD);
        translate([0, D_OUTER/2 + dy, Z1]) rotate([-90, 0, 0]) cylinder(h=extends, d=D_CAPSULE + deltaD);
    }
}

WOOD_DY = -8;

module wood()
{
    color("olive") difference() {
        capsule(10, WOOD_DY);
        tube(h=H, do=D_OUTER*2, di=D_OUTER+EPS);
    }
}

//wood();
//case();

POST_DY = 5;
POWER_DY = 7.5;
SWITCH_DY = 9.5;

module attachPost()
{
    INSET = NUT_W * 0.6;

    difference() 
    {
        union() {
            simpleBridge(D_INNER, R_INNER - POST_DY, 5, MID_BRIDGE_DZ);
        }
        // Bolt hole
        rotate([-90, 0, 0])
            cylinder(h=20, d=BOLT_D);

        // Nut retainer
        translate([-NUT_W/2, 5, -20])
            cube(size=[NUT_W, NUT_Y, 40]);

        translate([-INSET/2, 0, -20])
            cube(size=[INSET, 6, 40]);
    }
}

difference() {
    union() {
        translate([0, 0, Z_MID])
            attachPost();

        // Switch holder. Fixme: needs side holders?
        translate([0, 0, Z_MID + DZ_SWITCH])
            simpleBridge(D_INNER, R_INNER - SWITCH_DY, 4, SWITCH_BRIDGE_DZ);

        // Power holder.
        translate([0, 0, Z_MID + DZ_PORT]) {
            simpleBridge(D_INNER, R_INNER - POWER_DY, 3, 12, 4);
        }
    }
    wood();
    // Underside of power port
    translate([0, 0, Z_MID + DZ_PORT]) {
        rotate([-90, 0, 0]) {
            // Initially measured.
            cylinder(h=50, d=8.0);
            cylinder(h=R_INNER - POWER_DY - 1.5, d=11.5);
        }
    }
}


difference() {
    DY = 12;
    BAFSIZE = 4;
    NBAF = 8;

    union() {
        // Baffles.
        for(i=[0:NBAF-1])
            translate([0, 0, 20 + i*BAFSIZE*2]) {
                difference() {
                    oneBaffle(D_INNER, BAFSIZE, bridge=(i<(NBAF-1)), battery=true, mc=false, cutout=false);
                    cylinder(h=BAFSIZE*1.1, d=D_INNER * 0.7);
                }
            }
        intersection() {
            capsule(10, WOOD_DY-4, 4);
            cylinder(h=100, d=D_INNER);
        }
    }
    wood();
    capsule(10, WOOD_DY-8, -4);
    
    // Flat bottom
    translate([-50, -R_INNER, 0]) cube(size=[100, 1.5, 200]);
}
