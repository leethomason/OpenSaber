include <dim.scad>
use <../shapes.scad>

$fn = 80;
EPS = 0.01;

T = 4;
DEPTH = 15.0;
M_TUBE = 10.0;
M_INNER = 3.0;
M_INSET = 11.0;

W_INSET = 13.5;
T_INSET = 1.0;

D_HOLDER = 8.0;

module holes()
{
    translate([(D_COUPLER - D_TUBE)/2, 0, 0])
        cylinder(h=100, d=D_TUBE_INNER);
    translate([(D_COUPLER - D_TUBE)/2, 0, M_TUBE])
        cylinder(h=100, d=D_TUBE);

}

module cap(dzRing, trimTop)
{
    difference() {
        union() {
            tube(h=dzRing, do=D_RING, di=D_COUPLER - T);
            tube(h=DEPTH, do=D_COUPLER, di=D_COUPLER - T);

            intersection() {
                cylinder(h=DEPTH, d=D_COUPLER);
                union() {
                    //translate([6.0, -50, 6]) cube(size=[100, 100, 100]);
                    translate([(D_COUPLER - D_TUBE)/2, 0, 0]) cylinder(h=100, d=D_HOLDER);
                    translate([-(D_COUPLER - D_TUBE)/2, 0, 0]) cylinder(h=100, d=D_HOLDER);
                }
            }

        }  
        holes();
        rotate([0, 0, 180]) holes();

        // Wire access
        cylinder(h=M_INNER, d=D_COUPLER - T);

        // Brass inset
        translate([-W_INSET/2, -50, M_INSET]) 
            cube(size=[W_INSET, 100, T_INSET]);

        if (trimTop) {
            translate([-50, D_HOLDER/2, 0]) cube(size=[100, 100, dzRing + EPS]);
        }
    }
}

//cap(DZ_RING0, true);
cap(DZ_RING1, false);

