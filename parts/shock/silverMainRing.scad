use <../threads.scad>
use <../shapes.scad>
include <dim.scad>

$fn = 90;

X_MC = 5;
Y_MC = 2;
MC_OFFSET = 4;
MC_WIDTH  = X_MC * PIN + 0.5;
MC_HEIGHT = Y_MC * PIN + 0.2;
H_LOCK = 4;
	
module portPin(h)
{	
	translate([-MC_WIDTH/2, MC_OFFSET, 0]) {
		cube(size=[MC_WIDTH, MC_HEIGHT, h]);

        LIP = 1.5;
        translate([LIP, -T_PIN_HOLDER_WALL, 0]) {
            cube(size=[MC_WIDTH - LIP*2, MC_HEIGHT + T_PIN_HOLDER_WALL, h]);
        }
	}
}

module portHolder(h, longY)
{
	Y = longY ? 40 : MC_HEIGHT + T_PIN_HOLDER_WALL*2;

	translate([-MC_WIDTH/2, MC_OFFSET, 0]) {
    	translate([-T_PIN_HOLDER_WALL, -T_PIN_HOLDER_WALL, 0]) {
			cube(size=[MC_WIDTH + T_PIN_HOLDER_WALL*2, Y, h]);
		}
	}
}

// Ring 
intersection()
{
	// Keep everything in the cylinder!
	translate([0, 0, -10]) {
		cylinder(h=H_RING+20, r=D_INNER/2);	
	}
	difference() {
		union() {
			// The ring.
			tube(H_RING, D_INNER/2 - T_RING, D_INNER/2);

			// switch support.
			BUFFER = 2;
			translate([-D_SWITCH/2 - 2, D_INNER/2 - 6, -BUFFER]) {
				cube(size=[D_SWITCH + 4, 10, H_RING + 2 * BUFFER]);
			}

			// Microcontroller port.
			rotate([0, 0, 180]) {
				translate([0, 0, H_RING - H_PIN_HOLDER-2]) {
					portHolder(H_PIN_HOLDER+2, true);
				}
			}
			// Emitter port
			rotate([0, 0, 90]) {
                translate([0, 0, H_LOCK]) {
                    emitterHolder(H_PIN_HOLDER);
                }
			}
            
            // Rod hard point.
            //flip(
            *translate([8, 1, H_RING - H_PIN_HOLDER-2]) {
                cube(size=[10, 10, H_PIN_HOLDER + 2]);
            }
		}

		// Hole drill.
		translate([0, 14, H_RING/2]) {
			rotate([-90, 0, 0]) {
				cylinder(d=16, h=20);
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
        
        // Remove lock
		rotate([0, 0, 90]) {
            lock(H_LOCK);
		}

        // Remove rod
        *translate([-X_ROD, Y_ROD, 0]) {
            cylinder(d=D_ROD, h=20, $fn=FACES);
        }

        
	}
}
