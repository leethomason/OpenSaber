use <../threads.scad>
use <../shapes.scad>
include <dim.scad>

$fn = 90;

H_RING = 16;
T_RING = 2.5;

PIN	   = 2.54;
H_PIN  = 16;
H_FEM_PIN_HOLDER = 8;
T_HOLDER_WALL = 2;

X_MC = 5;
Y_MC = 2;
WIDTH_MC = T_HOLDER_WALL * 2 + X_MC * PIN;
HEIGHT_MC = T_HOLDER_WALL * 2 + Y_MC * PIN;
OFFSET_MC = 15;

X_EMITTER = 4;
Y_EMITTER = 1;
WIDTH_EMITTER  = T_HOLDER_WALL * 2 + X_EMITTER * PIN;
HEIGHT_EMITTER = T_HOLDER_WALL * 2 + Y_EMITTER * PIN;
OFFSET_EMITTER = 10; 


module port(xPins, yPins)
{
	INNER_WIDTH  = xPins * PIN;
	INNER_HEIGHT = yPins * PIN;
	OUTER_WIDTH  = INNER_WIDTH  + T_HOLDER_WALL * 2;
	OUTER_HEIGHT = INNER_HEIGHT + T_HOLDER_WALL * 2; 

	translate([-OUTER_WIDTH/2, -OUTER_HEIGHT/2, 0]) {
		difference() {
			cube(size=[OUTER_WIDTH, OUTER_HEIGHT, H_FEM_PIN_HOLDER]);
			translate([T_HOLDER_WALL, T_HOLDER_WALL, 0]) {
				cube(size=[INNER_WIDTH, INNER_HEIGHT, H_FEM_PIN_HOLDER]);
			}
		}	
	}
}


port(4, 2);

/*
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

		*intersection() {
			cylinder(h=H_RING, d=D_INNER);

			// Microcontroller port.
			translate([-WIDTH_MC/2, -OFFSET_MC, H_RING - H_FEM_PINS]) {
				cube(size=[WIDTH_MC, H_FEM_PINS, H_FEM_PINS]);
			}	
			// Emitter port.
			*translate([OFFSET_EMITTER, -WIDTH_EMITTER/2, 0]) {
				cube(size=[HEIGHT_EMITTER, WIDTH_EMITTER, H_FEM_PINS]);
			}
		}
	}

	// Punch Microcontroller Port
	*translate([-(X_MC * PIN)/2, -OFFSET_MC + T_HOLDER_WALL, 0]) {
		cube(size=[X_MC * PIN, Y_MC * PIN, 40]);
	}	

}

*color("green") {
	translate([0, 0, H_RING/2]) {
		rotate([-90, 0, 0]) {
			cylinder(d=D_SWITCH, h=D_INNER/2 + 1);
		}
	}
}
*/
