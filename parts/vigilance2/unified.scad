use <../commonUnified.scad>
use <../shapes.scad>
use <../inset.scad>
use <chamber.scad>

include <dim.scad>

EPS = 0.005;
T = 4.0;

//$fn = 80; // overrides fs; num fragments
$fa = 6;   // minimum angle (default 12)
$fs = 0.8;  // minimum size (default 2)

module window()
{
    W = 3;
    H = 6;
    P = 3;

    points = [
        [-W, H], [0, H + P], [W, H], [W, -H], [0, -H - P], [-W, -H]
    ];
    translate([D0/2 - 5, 0, 0]) polygonZY(h=100, points=points); 
    mirror([-1, 0, 0]) translate([D0/2 - 5, 0, 0]) polygonZY(h=100, points=points); 
}


difference() {
    union() {
        rotate([0, 0, 180]) {
            translate([0, 0, M_BACK]) speakerHolder(D0, DZ_SPEAKER + EPS, 3.0, "std28");
            translate([0, 0, M_MC_BATTERY]) {
                baffleMCBattery2(D0, N_BATT_BAFFLES, DZ_BATTERY);
            }
        }
        translate([0, 0, M_MC_BATTERY + DZ_BATTERY]) {
            H = DZ_BOARDS - DZ_BATTERY;
            X_MC                = 0.7 * 25.4;
            Y_MC                = 9.0;
            D_BATTERY           = 18.5 + 0.5;
            MC_BASE =  D0/2 - D_BATTERY;
            TR0 = 10.0;
            TR1 = 14.0;
            
            difference() {
                cylinder(h=H, d=D0);
                translate([-X_MC/2, -MC_BASE, -EPS]) cube(size=[X_MC, 100, 100]);
                translate([-TR0/2, -12.0, -EPS]) cube(size=[TR0, 100, 100]);
            }
            // End stops
            intersection() {
                translate([0, 0, H]) cylinder(h=1.0, d=D0);
                difference() {
                    translate([-50, -50, -50]) cube(size=[100, 100, 100]);
                    translate([-TR1/2, -50, -EPS]) cube(size=[TR1, 100, 100]);
                }
            }
        }

        color("burlywood") translate([0, 0, M_PORT_RING]) {
            powerPortRing(D0, T,
                M_BOLT_RING - M_PORT_RING, 
                M_PORT - M_PORT_RING,
                counter=false);
        }
        color("cadetblue") translate([0, 0, M_BOLT_RING]) {
            boltRing(D0, T, M_TACTILE_RING - M_BOLT_RING, M_BOLT - M_BOLT_RING);
        }       
        color("coral") translate([0, 0, M_TACTILE_RING]) {
            tactileRing(
                D0, T, 
                M_RING_END - M_TACTILE_RING, 
                M_SWITCH - M_TACTILE_RING);
        }
        translate([0, 0, M_RING_END]) {
            tubeTriTop(h=2.0, do=D0, di=D0 - T);
        }
    }
    // Flat bottom
    translate([-50, -D0/2-EPS, -50]) cube(size=[100, 1.0, 1000]);

    // Flat bottom of the battery section, to give the slicer something
    // to put supports under.
    translate([-50, -D0/2, M_MC_BATTERY]) cube(size=[100, 6.0, DZ_BATTERY]);

    // Bottom access
    translate([0, -10, M_PORT]) hull() {
        D = 12.0;
        rotate([90, 0, 0]) cylinder(h=100, d=D);
        translate([0, 0, 18]) rotate([90, 0, 0]) cylinder(h=100, d=D);
    }

    // Side access
    translate([0, 0, M_PORT]) window();
}

translate([0, 0, M_CHAMBER]) {
    chamber();
}

*color("silver") translate([0, 0, M_WINDOW]) tube(h=DZ_WINDOW, do=D_OUTER, di=D0); 
color("red") translate([0, 0, M_FORE]) cylinder(h=1.0, d=D_OUTER);
color("red") translate([0, D_OUTER/2, M_WINDOW]) cylinder(h=DZ_WINDOW, d=1.0);