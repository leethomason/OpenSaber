use <../threads.scad>
use <../shapes.scad>
include <dim.scad>

$fn = 90;

H_RING = 16;
T_RING = 2.5;

X_MC = 5;
Y_MC = 2;
OFFSET_MC = 10;

X_EMITTER = 4;
Y_EMITTER = 1;
OFFSET_EMITTER = 12; 

PIN	   = 2.54;
H_PIN_HOLDER = 8;
T_HOLDER_WALL = 2;

module port(xPins, yPins, drawOuter)
{
	INNER_WIDTH  = xPins * PIN;
	INNER_HEIGHT = yPins * PIN;
	OUTER_WIDTH  = INNER_WIDTH  + T_HOLDER_WALL * 2;
	OUTER_HEIGHT = INNER_HEIGHT + T_HOLDER_WALL * 2; 

	translate([-OUTER_WIDTH/2, -OUTER_HEIGHT/2, 0]) {
		if (drawOuter) {
			difference() {
				cube(size=[OUTER_WIDTH, OUTER_HEIGHT, H_PIN_HOLDER]);
				translate([T_HOLDER_WALL, T_HOLDER_WALL, -1]) {
					cube(size=[INNER_WIDTH, INNER_HEIGHT, H_PIN_HOLDER + 2]);
				}
			}
		}	
		else {
			translate([T_HOLDER_WALL, T_HOLDER_WALL, -10]) {
				cube(size=[INNER_WIDTH, INNER_HEIGHT, H_PIN_HOLDER + 20]);
			}
		}
	}
}


//port(4, 2);


difference() 
{
	union() {
		difference() {
			tube(H_RING, D_INNER/2 - T_RING, D_INNER/2);
			translate([0, 0, H_RING/2]) {
				rotate([-90, 0, 0]) {
					cylinder(d=D_SWITCH, h=D_INNER/2 + 1);
				}
			}
		}

		intersection() {
			cylinder(h=H_RING, d=D_INNER);

			union() {
				// Microcontroller port.
				translate([0, -OFFSET_MC, H_RING - H_PIN_HOLDER]) {
					port(X_MC, Y_MC, true);
				}
				// Emitter port.
				rotate([0, 0, 90]) {
					translate([0, -OFFSET_EMITTER, 0]) {
						port(X_EMITTER, Y_EMITTER, true);
					}
				}
			}	
		}

	}

	// Punch Microcontroller Port
	translate([0, -OFFSET_MC, H_RING - H_PIN_HOLDER]) {
		port(X_MC, Y_MC, false);
	}
	// Punch emitter port.
	rotate([0, 0, 90]) {
		translate([0, -OFFSET_EMITTER, 0]) {
			port(X_EMITTER, Y_EMITTER, false);
		}
	}
}

*color("green") {
	translate([0, 0, H_RING/2]) {
		rotate([-90, 0, 0]) {
			cylinder(d=D_SWITCH, h=D_INNER/2 + 1);
		}
	}
}
