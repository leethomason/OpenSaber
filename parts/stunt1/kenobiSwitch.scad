$fn=90;

LOWER_H = 6.0;
UPPER_H = 2.4; // 4.4;
TOTAL_H = UPPER_H + LOWER_H;

SW_X = 6.5;
SW_Z = 4.0;

LOWER_D = 9.5; // 9.35;
UPPER_D = 14.0;
OUTER_D = 38.0;

module centerPost() {
    cylinder(h=20, d=3.8);
}

module inset() {
    INSET = 2;
    D = 8;
    translate([0, 0, TOTAL_H-INSET]) cylinder(h=10, d=D);
}

difference() {
    cylinder(d=LOWER_D, h=TOTAL_H);
    translate([-SW_X/2, -SW_X/2, 0]) cube(size=[SW_X, SW_X, SW_Z]);
    centerPost();
    inset();
}

difference() {
    union() {
        cylinder(d=UPPER_D, h=TOTAL_H);
    }
    translate([0, 10, -OUTER_D/2 + LOWER_H]) rotate([90, 0, 0]) cylinder(h=20, d=OUTER_D);   
    centerPost();
    inset();
}
