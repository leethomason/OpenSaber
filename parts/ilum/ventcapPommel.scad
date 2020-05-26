include <dim.scad>
include <dimCoupler.scad>
use <../shapes.scad>

$fn = 80;
EPS = 0.01;

DEPTH = 17.0;
AFT_DEPTH = 22.0;
M_TUBE = 10.0;
M_INNER = 3.0;
M_INSET = 13.6 - 4.05;

W_INSET = 13.5;
T_INSET = 1.0;

DZ_AFT_TUBE = 3.0;  // AFT_DEPTH - (2.0 * H_COUPLER_PCB + TOOTH_HEIGHT);
W_PCB_CUT = 16.0;
PCB_SLOP = 0.6;

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

module cap(dzRing)
{
    depth = AFT_DEPTH;

    difference() {
        union() {
            tube(h=dzRing, do=D_RING, di=D_VENT_INNER);
            tube(h=depth, do=D_VENT, di=D_VENT_INNER);

            intersection() {
                cylinder(h=depth, d=D_VENT);
                union() {
                    translate([(D_VENT - D_TUBE)/2, 0, AFT_DEPTH - DZ_AFT_TUBE]) 
                        cylinder(h=DZ_AFT_TUBE, d=D_HOLDER);
                    translate([-(D_VENT - D_TUBE)/2, 0, AFT_DEPTH - DZ_AFT_TUBE]) 
                        cylinder(h=DZ_AFT_TUBE, d=D_HOLDER);
                }
            }
        }  
        aftHoles();
        rotate([0, 0, 180]) aftHoles();

        // Wire access
        cylinder(h=M_INNER, d=D_VENT_INNER);

        ROTATE = 30;
        WIDTH = W_INSET + 2;
        POS = DZ_FORE_SUPPORT + H_COUPLER_PCB + COUPLER_PLASTIC_HEIGHT;

        rotate([0, 0, ROTATE]) {
            translate([-WIDTH/2, -50, POS - PCB_SLOP/2]) 
                cube(size=[WIDTH, 100, H_COUPLER_PCB + PCB_SLOP]);
        }

        // cutout to fit the switch plate
        translate([-50, D_HOLDER/2, 0]) cube(size=[100, 100, dzRing + EPS]);
    }
}

// Aft cap
cap(DZ_RING0);
