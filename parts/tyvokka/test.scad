include <dim.scad>
include <../shapes.scad>
include <../commonUnified.scad>

$fn=60;
EPS = 0.01;
EPS2 = EPS * 2;

H = 100;
Z0 = 20;
Z1 = 60;
DSLOT = 16.2;
DWOOD = 20;

BOLT_D = 4; // fixme
NUT_W = 8;  // fixme
NUT_Y = 4;  // fixme

module case(extend = 0)
{
    color("silver") difference() 
    {
        tube(h=H, do=D_OUTER + extend, di=D_INNER);
        hull() {
            translate([0, 0, Z0]) rotate([-90, 0, 0]) cylinder(h=100, d=DSLOT);
            translate([0, 0, Z1]) rotate([-90, 0, 0]) cylinder(h=100, d=DSLOT);
        }
    }
}


module capsule(extends, dy, deltaD=0)
{
    hull() {
        translate([0, D_OUTER/2 + dy , Z0]) rotate([-90, 0, 0]) cylinder(h=extends, d=DSLOT + deltaD);
        translate([0, D_OUTER/2 + dy, Z1]) rotate([-90, 0, 0]) cylinder(h=extends, d=DSLOT + deltaD);
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

rotate([90, 0, 0]) translate([0, -10, -10]) wood();
//case();

module attachPost()
{
    DZ = 10;
    INSET = NUT_W * 0.6;

    difference() 
    {
        translate([-20, 2, -DZ/2]) {
            cube(size=[40, 20, DZ]);
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

/*
difference() {
    CZ = (Z1 + Z0) / 2;
    translate([0, 0, CZ])
        attachPost();
    wood();
    tube(h=100, do=D_OUTER*2, di=D_INNER+EPS);
}

difference() {
    DY = 12;
    BAFSIZE = 6.5;
    NBAF = 6;

    union() {
        // Baffles.
        for(i=[0:NBAF-1])
            translate([0, 0, i*BAFSIZE*2])
                oneBaffle(D_INNER, BAFSIZE, bridge=(i<(NBAF-1)));
        capsule(10, WOOD_DY-4, 4);
    }
    wood();
    capsule(10, WOOD_DY-8, -4);

    // Contain to cylinder
    tube(h=100, do=D_OUTER*2, di=D_INNER+EPS);
}
*/