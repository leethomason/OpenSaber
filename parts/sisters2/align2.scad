C = 25.4;
PIN = C / 10.0;

DX = 2.2 * C;
DY = 0.7 * C;
PINY = 0.6 * C;

PILLAR = 4.0;
PILLAR_Z = 5.0;
BASE = 2.0;
TARGET = 3.0;
EDGE = 0.4;
TRIM = 0.1;

EPS = 0.001;
STRUCTURE = 2.0;
ZSTRUCT = 1.0;
HANDLE = 10.0;

module pillar() {
    translate([-PILLAR/2, -PILLAR/2, 0])
        cube(size=[PILLAR, PILLAR, PILLAR_Z]);
}

module support(x, y, sx, sy, height) {
    x = x + 0.05*C;
    y = 0.05*C + PINY - y;

    T = 1.0;
    difference() {
        translate([x - sx/2 - T, y - sy/2 - T, 0]) 
            cube(size=[sx + 2*T, sy + 2*T, TARGET - height + EDGE]);
        translate([x - sx/2 - TRIM, y - sy/2 - TRIM, TARGET - height])
            cube(size=[sx + TRIM*2, sy + TRIM*2, 10.0]);
    }
    translate([x - STRUCTURE/2, 0, 0])
        cube(size=[STRUCTURE, DY, ZSTRUCT]);
}

difference() {
    union() {
        pillar();
        translate([DX, 0, 0]) pillar();
        translate([DX, DY, 0]) pillar();
        translate([0, DY, 0]) pillar();
    }
    T = 0.15;
    translate([-T, -T, BASE])
        cube(size=[DX + T*2, DY + T*2, 10]);
}

// Base
difference() {
    cube(size=[DX, DY, ZSTRUCT]);
    translate([STRUCTURE, STRUCTURE, -EPS]) 
        cube(size=[DX-STRUCTURE*2, DY-STRUCTURE*2, 100]);
}

// Finger holds
translate([DX/2 - HANDLE/2, DY, 0]) cube(size=[HANDLE, HANDLE, ZSTRUCT]);
translate([DX/2 - HANDLE/2, -HANDLE, 0]) cube(size=[HANDLE, HANDLE, ZSTRUCT]);

// accel
support(1.2904*C, 0.2708*C, 2.06 + 0.01, 2.06 + 0.1, 1.0);
// amp
support(0.1429*C, 0.3045*C, 3.10, 3.10, 0.8);

/*
// mosfet
support(1.8256*C, 0.4913*C, 1.7, 2.0, 0.7);
support(1.8288*C, 0.3369*C, 1.7, 2.0, 0.7);
support(1.8288*C, 0.2244*C, 1.7, 2.0, 0.7);
*/

