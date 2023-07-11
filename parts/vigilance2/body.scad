include <dim.scad>
use <../shapes.scad>
use <../commonUnified.scad>

//$fn = 80; // overrides fs; num fragments
$fa = 6;   // minimum angle (default 12)
$fs = 0.8;  // minimum size (default 2)

difference() {
    union() {
        color("silver") translate([0, 0, M_POMMEL_OUTER]) cylinder(h=-M_POMMEL_OUTER, d=D_OUTER);
        color("grey") cylinder(h=M_SECTION_1 - M_0, d=D_OUTER);
        color("silver") translate([0, 0, M_SECTION_1]) cylinder(h=M_SECTION_2 - M_SECTION_1, d=D_OUTER);
        color("grey") translate([0, 0, M_SECTION_2]) cylinder(h=M_SECTION_3 - M_SECTION_2, d=D_OUTER);
    }
    translate([0, 0, -100]) cylinder(h=1000, d=D0);

    // Rings
    for(i=[0:N_RINGS-1]) {
        translate([0, 0, M_FIRST_RING + i * DZ_RING_SPACE]) tube(do=40, di=D_RING, h=DZ_RING);
    }

    // Windows
    translate([0, 0, M_WINDOW]) {
        for(i=[0:N_WINDOW-1]) {
            rot = i * 360 / N_WINDOW;
            rotate([0, 0, rot]) {
                //cube(size=[DX_WINDOW, 100, DZ_WINDOW]);
                hull() {
                    translate([0, 0, DX_WINDOW/2]) rotate([-90, 0, 0]) cylinder(h=100, d=DX_WINDOW);
                    translate([0, 0, DZ_WINDOW - DX_WINDOW/2]) rotate([-90, 0, 0]) cylinder(h=100, d=DX_WINDOW);
                }
            }
        }
    }

    // Switch Plate
    translate([0, 0, M_SWITCH_PLATE]) {
        hull() {
            translate([0, 0, DX_SWITCH_PLATE/2]) rotate([-90, 0, 0]) cylinder(h=100, d=DX_SWITCH_PLATE);
            translate([0, 0, DZ_SWITCH_PLATE - DX_SWITCH_PLATE/2]) rotate([-90, 0, 0]) cylinder(h=100, d=DX_SWITCH_PLATE);
        }
    }

    // Bubble
    translate([0, 0, M_BUBBLE]) {
       translate([-DX_BUBBLE/2, 0, 0]) cube(size=[DX_BUBBLE, 100, DZ_BUBBLE]);
    }
}

OVER = 5.0;
translate([0, 0, M_WINDOW - OVER]) {
    difference() {
        color("goldenrod") tube(h=DZ_WINDOW + OVER, do=32.0, di=28.9);
        for(i=[0:N_WINDOW-1]) {
            NCUTS = 5;
            DZCUT = 5.0;
            DW = 2.5;

            rot = i * 360 / N_WINDOW;
            rotate([0, 0, rot]) 
            for(i=[0:NCUTS-1]) {
                translate([0, 0, OVER + DZ_WINDOW/2 - (NCUTS-1)/2 * DZCUT + i * DZCUT]) hull() {
                    translate([-DW, 0, 0]) rotate([-90, 0, 0]) cylinder(h=100, d=3.2);
                    translate([DW, 0, 0]) rotate([-90, 0, 0]) cylinder(h=100, d=3.2);
                }
            }
        }
    }
}

color("red") translate([0, 0, 10.0])    // guess
    mirror([0, -1, 0]) battery(D0);
