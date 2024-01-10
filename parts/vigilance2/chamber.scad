include <dim.scad>
use <../shapes.scad>
use <../commonUnified.scad>

$fa = 6;   // minimum angle (default 12)
$fs = 0.8;  // minimum size (default 2)

EPS = 0.01;
BIT = 3.2;

DRAW_BRASS = false;
DRAW_BASE_PLATE_1 = true;
DRAW_BASE_PLATE_2 = true;
DRAW_INNER_RING = true;
DRAW_OUTER_RING = true;
CUTOUT = false;

RING_DEPTH = 0.5;
RING0 = 10.0;
RING1 = 17.0;
RING2 = 24.0;

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
    // Threaded rod to secure the crystal (tension)
    for(r=[A_ANCHOR_ROD:180:A_ANCHOR_ROD + 180]) {
        rotate([0, 0, r]) {
            translate([R_ANCHOR_ROD, 0, M_END - EPS]) {
                cylinder(h=100, d=D_ANCHOR_ROD);
            }
        }
    }
    // Tube to run wires (compression)
    for(r=[A_TUBE_ROD:180:A_TUBE_ROD + 180]) {
        rotate([0, 0, r]) {
            translate([R_TUBE_ROD, 0, M_END - EPS]) {
                cylinder(h=100, d=DI_TUBE_ROD);
                LEN = (M_BRASS_END - M_END) - DEPTH_TUBE_ROD - T_TOP_RING / 2;
                translate([0, 0, DEPTH_TUBE_ROD]) cylinder(h=LEN, d=DO_TUBE_ROD);
            }
        }
    }
    // Threaded rod to secure outer ring (tension) with tube coverings
    for(r=[A_BOLT_ROD:180:A_BOLT_ROD + 180]) {
        rotate([0, 0, r]) {
            translate([R_BOLT_ROD, 0, M_END - EPS]) {
                cylinder(h=100, d=D_BOLT_ROD);
                cylinder(h=DEPTH_HEAD_BOLT + EPS*2, d=D_HEAD_BOLT);
            }
            translate([R_BOLT_ROD, 0, M_END + DZ_CHAMBER_BASE_PLATE - RING_DEPTH])
                cylinder(h=10.0, d=DO_TUBE_ROD);
        }
    }
}

DI_BRASS = 29.0;    // 29.2 nominal
DO_BRASS = min(32.0, D0);

module chamber()
{
    difference() {
        if (DRAW_BRASS) {
            union() {
                // outer brass 
                difference() {
                    color("goldenrod") translate([0, 0, M_BRASS_START]) tube(h=DZ_BRASS, do=DO_BRASS, di=DI_BRASS);
                    for(i=[0:N_WINDOW-1]) {
                        DZCUT = 5.4;
                        DW = 3.5; //2.5;
                        NCUTS = 5;

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
            if (DRAW_BASE_PLATE_1) {
                translate([0, 0, M_END]) cylinder(h = AL_STOCK_3, d = D0);
            }
            if (DRAW_BASE_PLATE_2) {
                translate([0, 0, M_END + AL_STOCK_3]) cylinder(h = AL_STOCK_3, d = DI_BRASS);
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
        // LED tube
        translate([0, 0, M_END - EPS]) cylinder(h=10, d=7.0);

        rods();        

        // Alignment
        translate([-BIT/2, -50, M_END - EPS]) cube(size=[BIT, 50, 2]);

        // base plates fancy cuts
        translate([0, 0, M_END + DZ_CHAMBER_BASE_PLATE - RING_DEPTH]) {
            tube(h=10, di=RING0, do=RING0 + BIT);
            tube(h=10, di=RING1, do=RING1 + BIT);
        }

        for (it=[0:1]) {
            for(r=[0:3]) {
                if (r != 2) {
                    rotate([0, 0, -30 + r*20 + 180*it]) 
                        translate([RING2/2, 0, M_END + DZ_CHAMBER_BASE_PLATE - RING_DEPTH]) 
                            cylinder(h=1, d=BIT);
                }
            }
        }
    }

    // crystal
    *color("green") translate([0, 0, M_BRASS_START + DZ_BRASS / 2 - DZ_CRYSTAL / 2]) crystal();

    echo("chamber front=", M_BRASS_END + T_BOLT_RING, "stop=", M_STOP);
}

DRAW_HOLES = true;

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
        if (DRAW_HOLES) {
            translate([0, 0, M_PORT]) rotate([-90, 0, 0]) cylinder(h=100, d=D_SWITCH_PLATE_HOLE);
            translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=100, d=8.2);
            translate([0, D_OUTER/2 - 2.0, M_BOLT]) rotate([-90, 0, 0]) cylinder(h=100, d=8.0);
            translate([0,0, M_BOLT]) rotate([-90, 0, 0]) cylinder(h=100, d=4.4);
        }
    }
}

difference() {
    chamber();
    if (CUTOUT) {
        color("white") translate([-100, -100, 0]) cube(size=[100, 200, 1000]);
    }
}

*switchPlate();
*echo("SWITCH plate max height", DY_SWITCH_PLATE);