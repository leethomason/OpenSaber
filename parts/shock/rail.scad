include <dim.scad>

H_RAIL = 55;
N = 4;
H = 3;
T = 2;

$fn = 180;

difference() {
    intersection() 
    {
        translate([0, D_INNER/2, 0]) {
            cylinder(h=H_RAIL, r=D_INNER/2);
        }
        translate([-W_NOTCH/2, 0, 0]) {
            cube(size=[W_NOTCH, H_NOTCH + 2, H_RAIL]);
        }
    }
    for(i=[0:N-1]) {
        h = (H_RAIL - H_BUTTRESS) * i / (N - 1);
        translate([-W_NOTCH/2, H_NOTCH, h]) {
            cube(size=[W_NOTCH, T, H]);
        }
    }
}