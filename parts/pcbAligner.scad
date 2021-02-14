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

OUTSIDE = true;

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
    translate([x - STRUCTURE/2, -1, 0])
        cube(size=[STRUCTURE, DY + 2, ZSTRUCT]);
}

difference() {
    union() {
        pillar();
        translate([DX, 0, 0]) pillar();
        translate([DX, DY, 0]) pillar();
        translate([0, DY, 0]) pillar();
    }
    T = 0.15;
    T1 = 0.35;
    hull() {
        translate([-T, -T, BASE])
            cube(size=[DX + T*2, DY + T*2, 10]);
        translate([-T1, -T1, PILLAR])
            cube(size=[DX + T1*2, DY + T1*2, 10]);
    }
}

if (OUTSIDE) {
    OUT = 3.5;
    OUT0 = 0.5;

    // Base
    difference() {
        translate([-OUT, -OUT, 0])
            cube(size=[DX + OUT*2, DY + OUT*2, ZSTRUCT]);
        translate([-OUT0, -OUT0, -EPS]) 
            cube(size=[DX+OUT0*2, DY+OUT0*2, 100]);
    }

    // Finger holds
    translate([DX/2 - HANDLE/2, DY, 0]) cube(size=[HANDLE, HANDLE, ZSTRUCT]);
    translate([DX/2 - HANDLE/2, -HANDLE, 0]) cube(size=[HANDLE, HANDLE, ZSTRUCT]);

}
else {
    // Base
    difference() {
        cube(size=[DX, DY, ZSTRUCT]);
        translate([STRUCTURE, STRUCTURE, -EPS]) 
            cube(size=[DX-STRUCTURE*2, DY-STRUCTURE*2, 100]);
    }

    // Finger holds
    translate([DX/2 - HANDLE/2, DY, 0]) cube(size=[HANDLE, HANDLE, ZSTRUCT]);
    translate([DX/2 - HANDLE/2, -HANDLE, 0]) cube(size=[HANDLE, HANDLE, ZSTRUCT]);
}

// accel
support(1.2904*C, 0.2708*C, 2.06 + 0.01, 2.06 + 0.1, 1.0);

if (OUTSIDE == false) {
    // amp
    support(0.1429*C, 0.3045*C, 3.10, 3.10, 0.8);
}