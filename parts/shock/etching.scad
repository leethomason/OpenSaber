D_TUBE = 33.0;
R_TUBE = D_TUBE / 2;
L = 60;
W = 25;
H = 30;
T = 3;

$fn=90;

difference() {
    translate([-W/2-T, 0, 0]) {
        cube(size=[W + T*2, H + R_TUBE, L + T*2]);
    }
    translate([-W/2, 0, T]) {
        cube(size=[W, H + R_TUBE + 20, L]);
    }
    cylinder(h=L + T*2, r=D_TUBE/2);
}