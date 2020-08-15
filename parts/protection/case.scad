use <../shapes.scad>
include <dim.scad>

module dotstar() {
    for(i=[0:3]) {
        translate([0, 0, 7.0*i]) rotate([0, -90, 0]) cylinder(h=100, d=2.0);
    }
}

module brassSleeve(addOuter=0)
{
    color("Gold") 
    difference() {
        union() {
            translate([0, 0, M_BRASS])
                tube(h=DZ_BRASS, do=DO_BRASS + addOuter, di=DI_BRASS, $fn=60);
        }
        //rotate([0, 0, 180]) brassCenterCut();
        //echo("First dotstart, from top of BRASS", M_FIRST_DOTSTAR - (M_BRASS + DZ_BRASS));
        //translate([0, 0, M_FIRST_DOTSTAR]) dotstar();

        translate([0, 0, M_ALIGN]) rotate([90, 0, 0]) cylinder(h=100, d=4.2);

        TCUTBRASS = 19.0;
        echo("Brass switch cutout center d=", TCUTBRASS, "m=", M_SWITCH - M_ALIGN);
        hull() {
            translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=100, d=TCUTBRASS);
            translate([0, 0, 0]) rotate([-90, 0, 0]) cylinder(h=100, d=TCUTBRASS);
        }
    }
}

module copperChamber()
{
    color("DarkGoldenrod") 
    difference() {
        union() {
            translate([0, 0, M_COPPER])
                tube(h=DZ_COPPER, do=DO_COPPER, di=DI_COPPER, $fn=80);
        }
        W = 8.0;
        END_Z = 4.0;
        translate([-W/2, -50, M_COPPER + END_Z])
            cube(size=[W, 100, DZ_COPPER - END_Z*2]);
        //translate([0, 0, M_COPPER]) copperBaffles();
        //rotate([0, 0, 180]) translate([0, 0, M_COPPER]) copperBaffles();
    }
}
