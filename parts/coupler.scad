use <commonUnified.scad>
use <shapes.scad>

module coupler(diameter, dz, diameterHeatsink)
{
    $fn = 80;

    SIZE = [16.78, 0, 16.78];
    CENTER_X = 16.78 / 2;
    CENTER_Z = 16.78 / 2;

    CUBE_Y = 10;
    CUBE_Z = 5.0;

    MOUNT = [
        [ 2.04 - CENTER_X,  2.04, "buttress"],
        [14.74 - CENTER_X,  2.04, "buttress"],
        [ 2.04 - CENTER_X, 14.74, "buttress"],
        [14.74 - CENTER_X, 14.74, "buttress"],
    ];

    difference() {
        pcbHolder(diameter, 4, dz, 2, 0, SIZE, MOUNT, holeAccess=true);

        translate([-50, -100, 0]) cube(size=[100, 100 - diameter/2 + 0.5, 100]);
        translate([-SIZE[0]/2, 5, 4])
            cube(size=[SIZE[0], 100, dz - 8]);
    }

    OVERLAP = 2;
    difference() {
        translate([0, 0, dz - OVERLAP])
            translate([-40, -CUBE_Y/2, 0])
                cube(size=[80, CUBE_Y, CUBE_Z + OVERLAP]);

        tube(h=100, do=100, di=diameter);    
        translate([0, 0, dz])   
            tube(h=100, do=100, di=diameterHeatsink);
        cylinder(h=100, d=diameterHeatsink-4);
    }
}

//coupler(25.4, 24.0);
//coupler(30.0, 24.0);
coupler(29.0, 24.0, 21.0);
