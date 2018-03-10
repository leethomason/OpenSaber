H = 18;
D_INNER = 22;
D_OUTER = 26;
D_SWITCH = 12;

EPS = 0.01;
EPS2 = EPS*2;

CARR_X = 10;
CARR_Z = H - 4;

$fn = 60;

// Body.
*difference() {
	translate([0, 0, -H/2])	cylinder(h=H, d=D_OUTER);
	translate([0, 0, -H/2])	cylinder(h=H, d=D_INNER);
	rotate([-90, 0, 0]) {
		cylinder(h=D_OUTER, d=D_SWITCH);
	}	
}

// Payload
color("yellow") {
	rotate([-90, 0, 0]) {
		translate([0, 0, D_INNER/2-2]) cylinder(h=4, d=D_SWITCH);
	}	
}

// Carriage
*color("green") intersection() {
	translate([0, 0, -H/2])	cylinder(h=H, d=D_INNER);
	difference() {
		translate([-CARR_X/2, 0, -H/2 + (H - CARR_Z)/2]) cube(size=[10, 20, H-4]);
		translate([-4, 1, -H/2-EPS]) cube(size=[8, 5, H+EPS2]);
	}
}

// Holding ring
color("blue") difference() {
	difference() {
		translate([0, 0, -H/2])	cylinder(h=H, d=D_INNER);
		translate([0, 0, -H/2])	cylinder(h=H, d=D_INNER-2);
	}
	translate([-CARR_X/2, 0, -H/2 + (H - CARR_Z)/2]) cube(size=[10, 20, H-4]);
}
