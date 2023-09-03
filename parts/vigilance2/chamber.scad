include <dim.scad>
use <../shapes.scad>
use <../commonUnified.scad>

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
                color("goldenrod") tube(h=DZ_BRASS_CHAMBER, do=32.0, di=28.9);
                for(i=[0:N_WINDOW-1]) {
                    NCUTS = 5;
                    DZCUT = 5.0;
                    DW = 2.5;

                    rot = i * 360 / N_WINDOW;
                    rotate([0, 0, rot]) 
                    for(i=[0:NCUTS-1]) {
                        translate([0, 0, DZ_BRASS_CHAMBER/2 - (NCUTS-1)/2 * DZCUT + i * DZCUT]) hull() {
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