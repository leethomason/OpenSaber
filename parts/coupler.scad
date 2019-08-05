use <commonUnified.scad>

module coupler(diameter, dz)
{
    SIZE = [16.78, 0, 16.78];
    CENTER_X = 16.78 / 2;
    CENTER_Z = 16.78 / 2;

    MOUNT = [
        [ 2.04 - CENTER_X, 14.74, "buttress"],
        [14.74 - CENTER_X, 14.74, "buttress"],
        [ 2.04 - CENTER_X,  2.04, "buttress"],
        [14.74 - CENTER_X,  2.04, "buttress"],
    ];

    pcbHolder(diameter, 4, dz, 2, 0, SIZE, MOUNT, holeAccess=true);
}

//coupler(25.4, 24.0);
coupler(30.0, 24.0);

