use <commonUnified.scad>
use <shapes.scad>

module coupler(diameter, dz, diameterHeatsink, angle=0, dyPCB=0)
{
    $fn = 80;

    SIZE = [21.86, 100, 16.78];
    CENTER_X = SIZE[0] / 2;
    CENTER_Z = SIZE[2] / 2;

    CUBE_Y = 10;
    CUBE_Z = 5.0;

    MOUNT = [
        [ 2.04 - CENTER_X,  2.04, "buttress"],
        [19.82 - CENTER_X,  2.04, "buttress"],
        [ 2.04 - CENTER_X, 14.74, "buttress"],
        [19.82 - CENTER_X, 14.74, "buttress"],
    ];

    difference() {
        DZPCB = 3;
        pcbHolder(diameter, 4, dz, DZPCB, dyPCB, SIZE, MOUNT, holeAccess=true, angle=angle);

        // Cut top.
        translate([-SIZE[0]/2, 4, 0]) cube(size=[SIZE[0], 100, 100]);

        // Flat bottom
        translate([-50, -100, 0]) cube(size=[100, 100 - diameter/2 + 0.5, 100]);
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
coupler(29.0, 24.0, 21.0, angle=-20, dyPCB=-2);
