include <dim.scad>
use <../shapes.scad>

$fn = 80;

/*
H_ALIGNMENT = 10.0;

R_TUBE = 10.0;
THETA_TUBE = 0;
R_ROD = D_COUPLER/2 - D_BOLT_CAP/2;
THETA_ROD = 30.0;

module holes()
{
    rotate([0, 0, THETA_TUBE]) translate([R_TUBE, 0, 0]) 
        cylinder(h=H_ALIGNMENT, d=D_TUBE);
    rotate([0, 0, THETA_ROD]) translate([R_ROD, 0, 0]) 
        cylinder(h=H_ALIGNMENT, d=D_ROD);
}

difference() {
    cylinder(d=D_COUPLER, h=H_ALIGNMENT);
    cylinder(d=D_COUPLER_BOLT, h=H_ALIGNMENT);

    holes();
    rotate([0, 0, 180]) holes();
}
*/

T = 4;
DEPTH = 15.0;
DZ_TUBE = 8.0;
DZ_CLEAR = 4.0;

D_ONE_TUBE = 5.5;   // fixme
D_INNER_ONE_TUBE = 4.0; // fixme

difference() {
    union() {
        tube(h=DZ_RING0, do=D_RING, di=D_COUPLER - T);
        tube(h=DEPTH, do=D_COUPLER, di=D_COUPLER - T);

        intersection() {
            cylinder(h=DEPTH, d=D_COUPLER);
            translate([6.0, -50, 6]) cube(size=[100, 100, 100]);
        }

    }    
    translate([(D_COUPLER - D_ONE_TUBE)/2, 0, 0])
        cylinder(h=100, d=D_INNER_ONE_TUBE);
    translate([(D_COUPLER - D_ONE_TUBE)/2, 0, DEPTH - DZ_TUBE])
        cylinder(h=100, d=D_ONE_TUBE);
}


