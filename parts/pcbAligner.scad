INCH_TO_MM = 25.4;

// Offset from the origin (0,0) to the lower left of the board. Typically 1/2 of 1/10 of an inch
ORIGIN = 0.05 * INCH_TO_MM;
// Size of the board
DX = 2.2 * INCH_TO_MM;
DY = 0.7 * INCH_TO_MM;

PILLAR = 4.0;
PILLAR_Z = 5.0;
BASE = 2.0;
TARGET = 3.0;
SUPPORT_HEIGHT = 1.0;
EDGE = 0.6;

EPS = 0.001;
STRUCTURE = 3.0;
HANDLE = 10.0;

ZSTRUCT = 1.0;
ZSTRUCT_OUTER = 1.6;

TRIM = 0.35;        // Delta on the components in the holding pit
SIZE_TRIM = 0.5;    // Extra size on the board

module pillar() {
    translate([-PILLAR/2, -PILLAR/2, 0])
        cube(size=[PILLAR, PILLAR, PILLAR_Z]);
}

module support(x, y, sx, sy, height) {
    x = x + ORIGIN;
    y = DY - (y + ORIGIN);

    echo("support at:", x, y, "size:", sx, sy);

    T = 1.0;
    difference() {
        translate([x - sx/2 - T, y - sy/2 - T, 0]) 
            cube(size=[sx + 2*T, sy + 2*T, TARGET - height + EDGE]);
        translate([x - sx/2, y - sy/2, TARGET - height])
            cube(size=[sx, sy, 10.0]);
    }
    translate([x - STRUCTURE/2, -1, 0])
        cube(size=[STRUCTURE, DY + 2, ZSTRUCT]);
}

{
    difference() {
        union() {
            pillar();
            translate([DX, 0, 0]) pillar();
            translate([DX, DY, 0]) pillar();
            translate([0, DY, 0]) pillar();
        }
        hull() {
            translate([-SIZE_TRIM/2, -SIZE_TRIM/2, BASE])
                cube(size=[DX + SIZE_TRIM, DY + SIZE_TRIM, 10]);
            translate([-SIZE_TRIM/2, -SIZE_TRIM/2, PILLAR])
                cube(size=[DX + SIZE_TRIM, DY + SIZE_TRIM, 10]);
        }
    }

    OUT = 3.5;
    OUT0 = 0.5;

    // Base
    difference() {
        translate([-OUT, -OUT, 0])
            cube(size=[DX + OUT*2, DY + OUT*2, ZSTRUCT_OUTER]);
        translate([-OUT0, -OUT0, -EPS]) 
            cube(size=[DX+OUT0*2, DY+OUT0*2, 100]);
    }

    // Finger holds
    translate([DX/2 - HANDLE/2, DY, 0]) cube(size=[HANDLE, HANDLE, ZSTRUCT]);
    translate([DX/2 - HANDLE/2, -HANDLE, 0]) cube(size=[HANDLE, HANDLE, ZSTRUCT]);

    // Itsy V4 accel
    support(1.3*INCH_TO_MM, 0.2*INCH_TO_MM, 3.00 + TRIM, 2.50 + TRIM, 0.86);

    // Itsy V2/V3/V4 audio amp
    support(4.9, 7.1, 3.00 + TRIM, 3.00 + TRIM, 0.8);
}