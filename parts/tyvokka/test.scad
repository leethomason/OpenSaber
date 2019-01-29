include <dim.scad>
include <../shapes.scad>
include <../commonUnified.scad>

$fn=90;
EPS = 0.01;
EPS2 = EPS * 2;

H = 100;
Z0 = 20;
Z1 = 60;
DSLOT = 16.2;
DWOOD = 20;

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

module wood()
{
    DY = 8;
    color("olive") difference() {
        hull() {
            translate([0, D_OUTER/2-DY , Z0]) rotate([-90, 0, 0]) cylinder(h=DY, d=DSLOT);
            translate([0, D_OUTER/2-DY, Z1]) rotate([-90, 0, 0]) cylinder(h=DY, d=DSLOT);
        }
        tube(h=H, do=D_OUTER*2, di=D_OUTER+EPS);
    }
}

wood();
//case();
