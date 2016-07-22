use <../threads.scad>
use <../shapes.scad>
include <dim.scad>

$fn = 90;

X_MC = 5;
Y_MC = 2;
MC_OFFSET = 8;
MC_WIDTH  = X_MC * PIN;
MC_HEIGHT = Y_MC * PIN;
H_LOCK = 4;
	
module portPin(h)
{
	translate([-MC_WIDTH/2, MC_OFFSET, 0]) {
		cube(size=[MC_WIDTH, MC_HEIGHT, h]);
	}
}

module portHolder(h)
{
	translate([-MC_WIDTH/2, MC_OFFSET, 0]) {
    	translate([-T_PIN_HOLDER_WALL, -T_PIN_HOLDER_WALL, 0]) {
			cube(size=[MC_WIDTH + T_PIN_HOLDER_WALL*2, MC_HEIGHT + T_PIN_HOLDER_WALL*2, h]);
		}
	}
}

// Ring 
intersection()
{
	// Keep everything in the cylinder!
	cylinder(h=H_RING, r=D_INNER/2);	
	difference() {
		union() {
			// The ring.
			tube(H_RING, D_INNER/2 - T_RING, D_INNER/2);
			// Microcontroller port.
			rotate([0, 0, 180]) {
				translate([0, 0, H_RING - H_PIN_HOLDER]) {
					portHolder(H_PIN_HOLDER);
				}
			}
			// Emitter port
			rotate([0, 0, 90]) {
				emitterHolder(H_PIN_HOLDER);
			}
		}

		// Remove switch.
		translate([0, 0, H_RING/2]) {
			rotate([-90, 0, 0]) {
				cylinder(d=D_SWITCH, h=D_INNER/2 + 1);
			}
		}
		// Remove Microcontroller
		rotate([0, 0, 180]) {
			portPin(H_RING);
		}

		// Remove port.
		rotate([0, 0, 90]) {
			emitterPin(H_RING, true);
		}
	}
}

// Lock
color("gray") {
	translate([0, 0, -H_LOCK]) {
		rotate([0, 0, 90]) {
			difference() 
			{
				tube(H_LOCK, D_INNER/2 - T_RING, D_INNER/2);
				lock(H_LOCK);
			}
		}
	}
}
