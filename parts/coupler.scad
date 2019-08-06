use <commonUnified.scad>
use <shapes.scad>

module coupler(diameter, dz)
{
    $fn = 80;

    SIZE = [16.78, 0, 16.78];
    CENTER_X = 16.78 / 2;
    CENTER_Z = 16.78 / 2;

    DI_US_HEATSINK = 28.0;      // FIXME
    DO_TCSS_HEATSINK = 20.0;    // FIXME

    CUBE_Y = 10;
    CUBE_Z = 5.0;

    MOUNT = [
        [ 2.04 - CENTER_X, 14.74, "buttress"],
        [14.74 - CENTER_X, 14.74, "buttress"],
        [ 2.04 - CENTER_X,  2.04, "buttress"],
        [14.74 - CENTER_X,  2.04, "buttress"],
    ];

    pcbHolder(diameter, 4, dz, 2, 0, SIZE, MOUNT, holeAccess=true);

    OVERLAP = 1;
    difference() {
        translate([0, 0, dz - OVERLAP])
            translate([-40, -CUBE_Y/2, 0])
                cube(size=[80, CUBE_Y, CUBE_Z + OVERLAP]);

        tube(h=100, do=100, di=diameter);    
        translate([0, 0, dz])   
            tube(h=100, do=100, di=DI_US_HEATSINK);
        cylinder(h=100, d=DO_TCSS_HEATSINK);
    }
}

//coupler(25.4, 24.0);
//coupler(30.0, 24.0);
coupler(32.0, 22.0);
