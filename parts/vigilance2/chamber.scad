include <dim.scad>
use <../shapes.scad>
use <../commonUnified.scad>

$fa = 6;   // minimum angle (default 12)
$fs = 0.8;  // minimum size (default 2)

EPS = 0.01;
BIT = 3.2;

DRAW_BRASS = false;
DRAW_BASE_PLATE = true;
DRAW_INNER_RING = false;
DRAW_OUTER_RING = false;

module switchPlateSimple(trim)
{
    hull() {
        translate([0, D_OUTER/2 - DY_SWITCH_PLATE, M_SWITCH_PLATE_START + DX_SWITCH_PLATE/2 - trim]) rotate([-90, 0, 0]) 
            cylinder(h=100, d=DX_SWITCH_PLATE + trim*2);
        translate([0, D_OUTER/2 - DY_SWITCH_PLATE, M_SWITCH_PLATE_END - DX_SWITCH_PLATE/2 + trim]) rotate([-90, 0, 0]) 
            cylinder(h=100, d=DX_SWITCH_PLATE + trim*2);
    }
}

module crystal()
{
    polygonXY(h=DZ_CRYSTAL, points=[ 
        [-DX_CRYSTAL/2, 0],
        [-DX_CRYSTAL/4, -DY_CRYSTAL/2],
        [DX_CRYSTAL/4, -DY_CRYSTAL/2],
        [DX_CRYSTAL/2, 0],
        [DX_CRYSTAL/4, DY_CRYSTAL/2],
        [-DX_CRYSTAL/4, DY_CRYSTAL/2]
    ]);
}

module boltPlateNuts()
{
    for(r=[A_ANCHOR_ROD:180:A_ANCHOR_ROD + 180]) {
        rotate([0, 0, r]) {
            translate([R_ANCHOR_ROD, 0, M_END - EPS]) {
                cylinder(h=100, d=D_ROD_NUT);
            }
        }
    }
}

module rods()
{
    for(r=[A_ANCHOR_ROD:180:A_ANCHOR_ROD + 180]) {
        rotate([0, 0, r]) {
            translate([R_ANCHOR_ROD, 0, M_END - EPS]) {
                cylinder(h=100, d=D_ANCHOR_ROD);
                cylinder(h=DEPTH_HEAD_ANCHOR, d=D_HEAD_ANCHOR);
            }
        }
    }
    for(r=[A_TUBE_ROD:180:A_TUBE_ROD + 180]) {
        rotate([0, 0, r]) {
            translate([R_TUBE_ROD, 0, M_END - EPS]) {
                cylinder(h=100, d=DI_TUBE_ROD);
                LEN = (M_BRASS_END - M_END) - DEPTH_TUBE_ROD - T_TOP_RING / 2;
                translate([0, 0, DEPTH_TUBE_ROD]) cylinder(h=LEN, d=DO_TUBE_ROD);
            }
        }
    }
    for(r=[A_BOLT_ROD:180:A_BOLT_ROD + 180]) {
        rotate([0, 0, r]) {
            translate([R_BOLT_ROD, 0, M_END - EPS]) {
                cylinder(h=100, d=D_BOLT_ROD);
                cylinder(h=DEPTH_HEAD_BOLT, d=D_HEAD_BOLT);
            }
        }
    }
}

DI_BRASS = 28.9;
DO_BRASS = 32.0;

module chamber()
{
    difference() {
        if (DRAW_BRASS) {
            union() {
                // outer brass 
                difference() {
                    color("goldenrod") translate([0, 0, M_BRASS_START]) tube(h=DZ_BRASS, do=DO_BRASS, di=DI_BRASS);
                    for(i=[0:N_WINDOW-1]) {
                        NCUTS = 5;
                        DZCUT = 5.0;
                        DW = 3.5; //2.5;

                        rot = i * 360 / N_WINDOW;
                        rotate([0, 0, rot]) 
                        for(i=[0:NCUTS-1]) {
                            translate([0, 0, M_WINDOW + DZ_WINDOW/2 - (NCUTS-1)/2 * DZCUT + i * DZCUT]) hull() {
                                translate([-DW, 0, 0]) rotate([-90, 0, 0]) cylinder(h=100, d=3.2);
                                translate([DW, 0, 0]) rotate([-90, 0, 0]) cylinder(h=100, d=3.2);
                            }
                        }
                    }

                }
            }
        }
    }

    difference() {
        union() {
            if (DRAW_BASE_PLATE) {
                translate([0, 0, M_END]) cylinder(h = DZ_CHAMBER_BASE_PLATE, d = D0);
            }
            if (DRAW_INNER_RING) {
                translate([0, 0, M_TOP_RING]) tube(h=T_TOP_RING, di = 12.0, do = DI_BRASS);
            }
            if (DRAW_OUTER_RING) {
                // bolt ring
                difference() {
                    translate([0, 0, M_BRASS_END]) tube(h=T_BOLT_RING, di = 16.0, do = DO_BRASS);
                    boltPlateNuts();
                }
            }
        }
        // Outer brass holder 
        translate([0, 0, M_END + DZ_BRASS_OFFSET]) tube(h=20, di=DI_BRASS, do = 40);
        // LED tube
        translate([0, 0, M_END - EPS]) cylinder(h=10, d=7.0);

        rods();        

        // Alignment
        translate([-BIT/2, -50, M_END - EPS]) cube(size=[BIT, 100, 2]);

        // base plates fancy cuts
        *for(r=[30:60:150]) {
            rotate([0, 0, r])   
                translate([-20, -BIT/2, M_END + DZ_CHAMBER_BASE_PLATE - 1.0])
                    cube(size=[40, BIT, 10]);
        }
        translate([0, 0, M_END + DZ_CHAMBER_BASE_PLATE - 0.5]) {
            tube(h=10, di=10.0, do=10.0 + BIT);
            tube(h=10, di=19.0, do=19.0 + BIT);
        }
    }

    // crystal
    *color("green") translate([0, 0, M_BRASS_START + DZ_BRASS / 2 - DZ_CRYSTAL / 2]) crystal();
}

module switchPlate()
{
    difference() {
        intersection() {
            translate([0, 0, M_SWITCH_PLATE_START]) 
                cylinder(h=M_SWITCH_PLATE_END - M_SWITCH_PLATE_START, d=D_OUTER - 0.01);
            hull() {
                translate([0, D_OUTER/2 - DY_SWITCH_PLATE, M_SWITCH_PLATE_START + DX_SWITCH_PLATE/2]) rotate([-90, 0, 0]) 
                    cylinder(h=100, d=DX_SWITCH_PLATE);
                translate([0, D_OUTER/2 - DY_SWITCH_PLATE, M_SWITCH_PLATE_END - DX_SWITCH_PLATE/2]) rotate([-90, 0, 0]) 
                    cylinder(h=100, d=DX_SWITCH_PLATE);
            }
        }
        translate([0, 0, M_PORT]) rotate([-90, 0, 0]) cylinder(h=100, d=D_SWITCH_PLATE_HOLE);
        translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=100, d=8.0); // fixme
        translate([0, D_OUTER/2 - 3.0, M_BOLT]) rotate([-90, 0, 0]) cylinder(h=100, d=8.0); // fixme
        translate([0,0, M_BOLT]) rotate([-90, 0, 0]) cylinder(h=100, d=4.5); // fixme
    }
}

difference() {
    chamber();
    color("white") translate([-100, -100, 0]) cube(size=[100, 200, 1000]);
}