include <dim.scad>
include <dimCoupler.scad>
use <../shapes.scad>

$fn = 80;
EPS = 0.01;

DEPTH = 17.0;
M_TUBE = 10.0;
M_INNER = 3.0;
M_INSET = 13.6 - 4.05;

W_INSET = 13.5;
T_INSET = 1.0;

DZ_AFT_TUBE = 3.0;  // AFT_DEPTH - (2.0 * H_COUPLER_PCB + TOOTH_HEIGHT);
W_PCB_CUT = 16.0;
PCB_SLOP = 0.6;

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

module cap(dzRing)
{
    depth = DEPTH;
    PCB_BOTTOM = M_INSET + PCB_SLOP/2 + H_COUPLER_PCB;

    difference() {
        union() {
            tube(h=dzRing, do=D_RING, di=D_VENT_INNER);
            tube(h=depth, do=D_VENT, di=D_VENT_INNER);

            intersection() {
                cylinder(h=depth, d=D_VENT);
                union() {
                    translate([(D_VENT - D_TUBE)/2, 0, 0]) 
                        cylinder(h=100, d=D_HOLDER);
                    translate([-(D_VENT - D_TUBE)/2, 0, 0]) 
                        cylinder(h=100, d=D_HOLDER);
                }
            }

            // cylinders to hold bolts
            intersection()
            {
                cylinder(h=20, d=D_VENT);
                union() {
                    translate([0, (D_VENT - D_M2_HEAD)/2, 0])
                        cylinder(h=DEPTH, d=D_M2_HEAD+2);
                    translate([0, -(D_VENT - D_M2_HEAD)/2, 0])
                        cylinder(h=DEPTH, d=D_M2_HEAD+2);
                }
            }
        }  
        holes();
        rotate([0, 0, 180]) holes();

        // Wire access
        cylinder(h=M_INNER + 2, d=D_VENT_INNER);

        ROTATE = 0;
        WIDTH = W_INSET;
        POS = M_INSET;

        // Slot to hold the PCB
        intersection() {
            cylinder(h=100, d=D_VENT);
            translate([-W_INSET/2, -50, -EPS])
                cube(size=[W_INSET, 100, POS + PCB_SLOP/2 + H_COUPLER_PCB]);
        }
        
        // Slide in the brass tubes
        translate([(D_VENT - D_TUBE)/2, 0, DEPTH-2])
            cylinder(h=2 + EPS, d1=D_TUBE, d2=D_TUBE + 2);
        translate([-(D_VENT - D_TUBE)/2, 0, DEPTH-2])
            cylinder(h=2 + EPS, d1=D_TUBE, d2=D_TUBE + 2);

        // knock out the inside for the pcb to slide in (trims cylinders)
        translate([-W_INSET/2, -4, 0]) cube(size=[W_INSET, 8, 100]);

        translate([0, (D_VENT - D_M2_HEAD)/2, PCB_BOTTOM - EPS])
            cylinder(h=100, d=1.9);
        translate([0, -(D_VENT - D_M2_HEAD)/2, PCB_BOTTOM - EPS])
            cylinder(h=100, d=1.9);
    }
}

// Fore cap
cap(DZ_RING1);
