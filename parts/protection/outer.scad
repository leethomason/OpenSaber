use <../shapes.scad>
include <dim.scad>

$fn = 80;

BIT = 3.175;
WING_DZ = 3;

echo("M_DESIGN_MAX_THREAD", M_DESIGN_MAX_THREAD);
echo("M_DESIGN_MAX_COUPLER", M_DESIGN_MAX_COUPLER);
if (M_DESIGN_MAX_COUPLER <= M_DESIGN_MAX_THREAD) echo("ERROR coupler impinging on thread");

BASE = 12.0 + BIT / 2;
DZ_GEM = M_DESIGN_MAX - (M_DESIGN_MIN + BASE) - BIT/2;

module dotstar() {
    for(i=[0:3]) {
        translate([0, 0, 7.0*i]) rotate([0, -90, 0]) cylinder(h=100, d=2.0);
    }
}

module centerCut()
{
    H = 20;
    DX = 8.0;

    TOP = 7 * 25.4;
    BOTTOM = (M_DESIGN_MIN + BASE) - TOP;

    echo("CENTER Aluminum cut, from top of section. On tool centers.");
    echo("P0", 0, BOTTOM);
    echo("P1", DX, BOTTOM + DZ_GEM/2);
    echo("P2", 0, BOTTOM + DZ_GEM);
    echo("P3", -DX, BOTTOM + DZ_GEM/2);

    hull() {
        translate([0, 0, 0]) rotate([-90, 0, 0]) cylinder(h=H, d=BIT);
        translate([DX, 0, DZ_GEM/2]) rotate([-90, 0, 0]) cylinder(h=H, d=BIT);
        translate([0, 0, DZ_GEM]) rotate([-90, 0, 0]) cylinder(h=H, d=BIT);
        translate([-DX, 0, DZ_GEM/2]) rotate([-90, 0, 0]) cylinder(h=H, d=BIT);
    }
}

module wing(print)
{
    MIDX = 6.0;

    DX0 = 10.0;
    DZ0 = 7.0;

    DX1 = 13.0;
    DZ1 = 17.0;
    H = 20;

    TOP = 7 * 25.4;
    BOTTOM = (M_DESIGN_MIN + BASE - WING_DZ) - TOP;

    if (print) {
        echo("WING Aluminum cut, from top of saber body. On tool center.");
        echo("Right");
        echo("P0", -MIDX, BOTTOM);
        echo("P1", -MIDX + DX0, BOTTOM + DZ0);
        echo("P2", -MIDX + DX1, BOTTOM + DZ1);
        echo("Left");
        echo("P0", -MIDX, BOTTOM);
        echo("P1", -MIDX - DX0, BOTTOM + DZ0);
        echo("P2", -MIDX - DX1, BOTTOM + DZ1);
    }

    hull() {
        translate([-MIDX, 0, 0]) rotate([-90, 0, 0]) cylinder(h=H, d=BIT);
        translate([-MIDX + DX0, 0, DZ0]) rotate([-90, 0, 0]) cylinder(h=H, d=BIT);
        translate([-MIDX + DX1, 0, DZ1]) rotate([-90, 0, 0]) cylinder(h=H, d=BIT);
    }

}

module brassCenterCut()
{
    H = 20;
    DX = 6.0;
    ZOFF = 1.0;

    M_TOP = M_BRASS + DZ_BRASS;
    BOTTOM = (M_DESIGN_MIN + BASE) - M_TOP;

    echo("Brass center cut, from top of brass section. On tool centers.");
    echo("Length of section", DZ_BRASS);
    echo("P0", 0, BOTTOM + ZOFF);
    echo("P1", DX, BOTTOM + DZ_GEM/2);
    echo("P2", 0, BOTTOM + DZ_GEM - ZOFF);
    echo("P3", -DX, BOTTOM + DZ_GEM/2);

    translate([0, 0, M_DESIGN_MIN + BASE]) hull()
    {
        translate([0, 0, ZOFF]) rotate([-90, 0, 0]) cylinder(h=H, d=BIT);
        translate([DX, 0, DZ_GEM/2]) rotate([-90, 0, 0]) cylinder(h=H, d=BIT);
        translate([0, 0, DZ_GEM - ZOFF]) rotate([-90, 0, 0]) cylinder(h=H, d=BIT);
        translate([-DX, 0, DZ_GEM/2]) rotate([-90, 0, 0]) cylinder(h=H, d=BIT);
    }
}

module switchCut()
{
    hull() {
        translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=100, d=D_CAPSULE);
        translate([0, 0, M_PORT]) rotate([-90, 0, 0]) cylinder(h=100, d=D_CAPSULE);
    }
}

// Outer case
difference() {
    union() {
        color("Gainsboro") tube(h=M_BODY_END - M_0, do=D_OUTER, di=D_INNER);
        *color("LightGray") translate([0, 0, M_BODY_END]) tube(h=M_EXT_END - M_BODY_END, do=D_OUTER, di=D_INNER);

        color("LightGray") translate([0, -D_OUTER/2 + 2, M_ALIGN])
            rotate([90, 0, 0])
                cylinder(h=4, d=6);
    }

    translate([0, 0, M_DESIGN_MIN + BASE]) centerCut();

    WING_ROTATE = -38; //-18;

    translate([0, 0, M_DESIGN_MIN + BASE - WING_DZ]) rotate([0, 0, WING_ROTATE])
        wing(true);
    translate([0, 0, M_DESIGN_MIN + BASE - WING_DZ]) rotate([0, 0, -WING_ROTATE])
        mirror([-1, 0, 0]) wing();

    rotate([0, 0, 180]) {
        translate([0, 0, M_DESIGN_MIN + BASE]) centerCut();
        translate([0, 0, M_DESIGN_MIN + BASE - WING_DZ]) rotate([0, 0, WING_ROTATE])
            wing();
        translate([0, 0, M_DESIGN_MIN + BASE - WING_DZ]) rotate([0, 0, -WING_ROTATE])
            mirror([-1, 0, 0]) wing();
    }

    //color("RoyalBlue")
    color("LightGray") for(i=[0:N_OUTER_RING-1]) {
        translate([0, 0, OUTER_RING_START + i * OUTER_RING_SPACE]) {
            tube(h=OUTER_RING_WIDTH, do=50, di=D_OUTER - OUTER_RING_DEPTH*2);
        }
    }

    switchCut();

    *color("plum") translate([0, 0, M_DESIGN_MAX]) tube(h=1.0, do=50, di=D_OUTER - OUTER_RING_DEPTH*2);

    translate([0, 0, M_FIRST_DOTSTAR]) dotstar();
}

// Brass
color("Gold") difference() {
    union() {
        translate([0, 0, M_BRASS])
            tube(h=DZ_BRASS, do=DO_BRASS, di=DI_BRASS);
    }
    brassCenterCut();
    rotate([0, 0, 180]) brassCenterCut();
    echo("First dotstart, from top of BRASS", M_FIRST_DOTSTAR - (M_BRASS + DZ_BRASS));
    translate([0, 0, M_FIRST_DOTSTAR]) dotstar();
}

// Copper
BAFFLE_Z = 9.6;
BAFFLE_STEP = 5.5;
N_BAFFLE = 5;

module copperBaffles()
{
    IW = 50.0;
    DY = 9.5;
    for(i=[0:N_BAFFLE - 1]) {
        translate([0, 0, BAFFLE_Z + i*BAFFLE_STEP]) {
            hull() {
                translate([-IW/2, DY, 0]) rotate([-90, 0, 0]) cylinder(h=20, d=BIT);
                translate([IW/2, DY, 0]) rotate([-90, 0, 0]) cylinder(h=20, d=BIT);
            }
        }
    }
    //translate([-50, 11.5, 0]) cube(size=[100, 100, 100]);
}

color("DarkGoldenrod") difference() {
    union() {
        translate([0, 0, TUBE_MIN])
            tube(h=DZ_COPPER, do=DO_COPPER, di=DI_COPPER);
    }
    W = 8.0;
    END_Z = 4.0;
    translate([-W/2, -50, TUBE_MIN + END_Z])
        cube(size=[W, 100, DZ_COPPER - END_Z*2]);
    translate([0, 0, TUBE_MIN]) copperBaffles();
    rotate([0, 0, 180]) translate([0, 0, TUBE_MIN]) copperBaffles();
    switchCut();
}

// Crystal
color("crimson") {
    CRYSTAL = 10.0;
    translate([0, 0, TUBE_MIN])
        rotate([0, 0, 45])
            translate([-CRYSTAL/2, -CRYSTAL/2, 0])
                cube(size=[CRYSTAL, CRYSTAL, M_DESIGN_MAX - TUBE_MIN]);
}

// Coupler:
color("plum") translate([0, 0, M_PCB_COUPLER]) cylinder(d=D_INNER, h=10.0);
