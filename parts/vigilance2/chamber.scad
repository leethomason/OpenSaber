include <dim.scad>
use <../shapes.scad>
use <../commonUnified.scad>

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

module chamber()
{
    difference() {
        union() {
            // outer brass 
            difference() {
                color("goldenrod") translate([0, 0, M_BRASS_START]) tube(h=DZ_BRASS, do=32.0, di=28.9);
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
        translate([0, -50, -0.1]) cube(size=[100, 100, 100]);
    }
    // crystal
    color("green") translate([0, 0, 0]) crystal();
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