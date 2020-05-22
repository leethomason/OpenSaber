include <dim.scad>
include <dimCoupler.scad>
use <../shapes.scad>

$fn = 80;
EPS = 0.01;

DEPTH = 15.0;
AFT_DEPTH = 22.0;
M_TUBE = 10.0;
M_INNER = 3.0;
M_INSET = 13.6 - 4.05;

W_INSET = 13.5;
T_INSET = 1.0;

DZ_AFT_TUBE = 3.5;  // AFT_DEPTH - (2.0 * H_COUPLER_PCB + TOOTH_HEIGHT);
W_PCB_CUT = 16.0;
PCB_SLOP = 0.3;

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
    hull() {
        translate([0, 0, DZ_FORE_SUPPORT + H_COUPLER_PCB + COUPLER_PLASTIC_HEIGHT - PCB_SLOP/2]) {
            cylinder(h=H_COUPLER_PCB + PCB_SLOP, d=D_COUPLER_DISC + 0.2);
        }
        translate([0, 30, DZ_FORE_SUPPORT + H_COUPLER_PCB + COUPLER_PLASTIC_HEIGHT - PCB_SLOP/2]) {
            cylinder(h=H_COUPLER_PCB + PCB_SLOP, d=D_COUPLER_DISC + 0.2);
        }
    }
    echo("COUPLER_PLASTIC_HEIGHT", COUPLER_PLASTIC_HEIGHT)
    echo("pcb", DZ_FORE_SUPPORT + H_COUPLER_PCB + COUPLER_PLASTIC_HEIGHT);
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
            // Brass inset - now PCB
            translate([-W_INSET/2, -50, M_INSET - PCB_SLOP/2]) 
                cube(size=[W_INSET, 100, H_COUPLER_PCB + PCB_SLOP]);
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
        cap(DZ_RING0, true);
    }
    pcbHolder();
}

// Fore cap
*union() {
    cap(DZ_RING1, false);
}
