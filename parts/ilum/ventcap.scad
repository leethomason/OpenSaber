include <dim.scad>
include <dimCoupler.scad>
use <../shapes.scad>

$fn = 80;
EPS = 0.01;

DEPTH = 15.0;
AFT_DEPTH = 20.0;
M_TUBE = 10.0;
M_INNER = 3.0;
M_INSET = 11.0;

W_INSET = 13.5;
T_INSET = 1.0;

DZ_AFT_TUBE = 3.5;
W_PCB_CUT = 16.0;

module holes()
{
    translate([(D_VENT - D_TUBE)/2, 0, 0])
        cylinder(h=100, d=D_TUBE_INNER);
    translate([(D_VENT - D_TUBE)/2, 0, M_TUBE])
        cylinder(h=100, d=D_TUBE);

}


module aftHoles()
{
    translate([(D_VENT - D_TUBE)/2, 0, 0])
        cylinder(h=100, d=D_TUBE_INNER);
    translate([(D_VENT - D_TUBE)/2, 0, AFT_DEPTH - DZ_AFT_TUBE])
        cylinder(h=100, d=D_TUBE);

}

module pcbHolder()
{
    /*
    intersection() {
        cylinder(h=200, d=D_VENT);
        color("plum") 
            translate([-W_PCB_CUT/2, -50, M_COUPLER_START + H_COUPLER_PCB + TOOTH_HEIGHT]) 
                cube(size=[W_PCB_CUT, 100, H_COUPLER_PCB]);
    }
    */

    hull() {
        translate([0, 0, M_COUPLER_START + H_COUPLER_PCB + TOOTH_HEIGHT]) {
            cylinder(h=H_COUPLER_PCB, d=D_COUPLER_DISC);
        }
        translate([0, 30, M_COUPLER_START + H_COUPLER_PCB + TOOTH_HEIGHT]) {
            cylinder(h=H_COUPLER_PCB, d=D_COUPLER_DISC);
        }
    }
}

module cap(dzRing, isAft)
{
    depth = isAft ? AFT_DEPTH : DEPTH;

    difference() {
        union() {
            tube(h=dzRing, do=D_RING, di=D_VENT_INNER);
            tube(h=depth, do=D_VENT, di=D_VENT_INNER);

            intersection() {
                cylinder(h=depth, d=D_VENT);
                union() {
                    if (isAft) {
                        translate([(D_VENT - D_TUBE)/2, 0, AFT_DEPTH - DZ_AFT_TUBE]) cylinder(h=DZ_AFT_TUBE, d=D_HOLDER);
                        translate([-(D_VENT - D_TUBE)/2, 0, AFT_DEPTH - DZ_AFT_TUBE]) cylinder(h=DZ_AFT_TUBE, d=D_HOLDER);
                    }
                    else {
                        translate([(D_VENT - D_TUBE)/2, 0, 0]) cylinder(h=100, d=D_HOLDER);
                        translate([-(D_VENT - D_TUBE)/2, 0, 0]) cylinder(h=100, d=D_HOLDER);
                    }
                }
            }

        }  
        if (isAft) {
            aftHoles();
            rotate([0, 0, 180]) aftHoles();
        }
        else {
            holes();
            rotate([0, 0, 180]) holes();
        }

        // Wire access
        cylinder(h=M_INNER, d=D_VENT_INNER);

        if (!isAft) {
            // Brass inset
            translate([-W_INSET/2, -50, M_INSET]) 
                cube(size=[W_INSET, 100, T_INSET]);
        }

        if (isAft) {
            // cutout to fit the switch plate
            translate([-50, D_HOLDER/2, 0]) cube(size=[100, 100, dzRing + EPS]);
        }
    }
}

// Aft cap
difference() {
    union() {
        translate([0, 0, M_AFT_FRONT]) cap(DZ_RING0, true);
    }
    pcbHolder();
    translate([0, 0, M_COUPLER_START]) cylinder(d=D_COUPLER_OUTER, h=2.0 * H_COUPLER_PCB + TOOTH_HEIGHT - H_COUPLER_PCB);
}

// Fore cap
*union() {
    cap(DZ_RING1, false);
}
