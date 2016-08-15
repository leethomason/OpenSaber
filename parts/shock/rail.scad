include <dim.scad>

H_RAIL = 60;
N = 4;
H = 3;
T = 2;

cube(size=[W_NOTCH, H_NOTCH, H_RAIL]);
for(i=[0:N-1]) {
	h = (H_RAIL - H_BUTTRESS) * i / (N - 1);
	if (i > 0) {
		translate([0, H_NOTCH, h-H]) {
			cube(size=[W_NOTCH, T, H]);
		}
	}
	if (i < N-1) {
		translate([0, H_NOTCH, h+H_BUTTRESS]) {
			cube(size=[W_NOTCH, T, H]);
		}
	}
}