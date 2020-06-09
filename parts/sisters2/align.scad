C = 25.4;
PIN = C / 10.0;

DX = 1.5 * C;
DY = 0.7 * C;
PINY = 0.6 * C;
NDRILL = 18;

TRIM = 0.30;
HALF_TRIM = TRIM / 2;

PART_H = 2.2;
BRIDGE_Z = 1.5;
BRIDGE_DZ = PART_H - BRIDGE_Z;
TUBE_Z = 0.8;
TUBE_DZ = PART_H - TUBE_Z;
TUBE_T = 1.0;
SLOT = 1.0;

$fn = 20;

module bridge(x, y, size) {
    PAD_X = 5.0;
    difference() {
        union() {
            translate([x - PAD_X, -PIN, BRIDGE_Z])
                cube(size=[PAD_X * 2, 2*PIN + PINY, BRIDGE_DZ]);
            translate([x - size/2 - TUBE_T, y - size/2 - TUBE_T, TUBE_Z])
                cube(size=[size + TUBE_T*2, size + TUBE_T*2, TUBE_DZ]);
        }
        translate([x - size/2 - TRIM/2, y - size/2 - TRIM/2])
            cube(size=[size + TRIM, size + TRIM, 10]);
        translate([x - SLOT/2, y - size/2 - 2.0, 0])
            cube(size=[SLOT, size + 4.0, 10]);
    }
}

module pin() {
    cylinder(h=10, d=1.2);
    translate([0, 0, PART_H - 1]) cylinder(h=1.1, d1=1.0, d2=2.0);
}

difference() {
    union() {
        translate([-PIN, -PIN, 0])
            cube(size=[2*PIN + DX, 1.5*PIN, PART_H]);
        translate([0, PINY, 0]) mirror([0, -1, 0]) translate([-PIN, -PIN, 0])
            cube(size=[2*PIN + DX, 1.5*PIN, PART_H]);        

        // accel
        bridge(1.2904*C, 0.2708*C, 2.06);
        // amp
        bridge(0.1429*C, 0.3045*C, 3.10);
    }
    for(i=[0:NDRILL-1]) {
        translate([PIN * i, 0, 0]) pin();
        translate([PIN * i, PINY, 0]) pin();
    }
}

