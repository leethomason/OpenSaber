/*
    The back of the emitter.
    Holds the power port.
 */

include <buttress.scad>

POWER_X = 11;
POWER_Y = 14.5;
POWER_Z = 10;
POWER_H = 12; // fixme

T = 2;
OFFSET_Y = -0.4;
OFFSET_X = -1;

LED_X = 13;
LED_Y = 8;
LED_Z = 1.5;

CABLE_X = 8;

difference() {
    buttress(crystal=false, pcb=7, biasRight=false, upperWiring=true);
    
    // LED holder
    translate([-LED_X/2, CRYSTAL_Y - LED_Y/2, H_BUTTRESS - LED_Z]) {
        cube(size=[LED_X, LED_Y, LED_Z]);
    }    

    // Extra wiring
    translate([8.5, -2, 0]) cylinder(h=10, r=3, $fn=90);
    translate([-11, -1, 0]) cylinder(h=10, r=2, $fn=90);
}

// Power port holder
difference() {
    translate([-(POWER_X + T)/2 + OFFSET_X, OFFSET_Y-T, -(POWER_Z + T)]) {
        cube(size=[POWER_X + T, POWER_Y+T, POWER_Z + T]);
    }
    translate([-(POWER_X)/2 - T + OFFSET_X, OFFSET_Y, -(POWER_Z)]) {
        cube(size=[POWER_X + T, POWER_Y, POWER_Z]);
    }
}
