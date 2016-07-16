use <../threads.scad>
use <../shapes.scad>
include <dim.scad>

$fn = 90;

H_RING = 16;
T_RING = 2.5;
PIN	   = 2.54;

T_HOLDER_WALL = 2;
X_MC = 5;
Y_MC = 2;
X_EMITTER = 4;

WIDTH_MC = T_HOLDER_WALL * 2 + X_MC * PIN;
HEIGHT_MC = T_HOLDER_WALL * 2 + Y_MC * PIN;
OFFSET_MC = 15;

H_FEM_PINS = 8;

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
			translate([-WIDTH_MC/2, -OFFSET_MC, H_RING - H_FEM_PINS]) {
				cube(size=[WIDTH_MC, H_FEM_PINS, HEIGHT_MC]);
			}	
		}
	}

	// Punch Microcontroller Port
	translate([-(X_MC * PIN)/2, -OFFSET_MC + T_HOLDER_WALL, 0]) {
		cube(size=[X_MC * PIN, Y_MC * PIN, 40]);
	}	
}

color("green") {
	translate([0, 0, H_RING/2]) {
		rotate([-90, 0, 0]) {
			cylinder(d=D_SWITCH, h=D_INNER/2 + 1);
		}
	}
}

