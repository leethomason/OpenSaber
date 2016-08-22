/*
    The back of the emitter.
    Holds the power port.
 */

include <buttress.scad>

POWER_X = 11;
POWER_Y = 14.5;
POWER_Z = 9;
POWER_H = 12; // fixme

T = 2;
OFFSET_Y = -0.2;

LED_X = 13;
LED_Y = 8;
LED_Z = 1.5;

CABLE_X = 8;

difference() {
    part(crystal=false, pcb=7, biasRight=false, upperWiring=true);
    
    // LED holder
    translate([-LED_X/2, CRYSTAL_Y - LED_Y/2, H_BUTTRESS - LED_Z]) {
        cube(size=[LED_X, LED_Y, LED_Z]);
    }        
}



*cylinder(r=D_INNER/2, h=1);

// Power port holder
difference() {
    translate([-(POWER_X + T)/2, OFFSET_Y-T, -(POWER_Z + T)]) {
        cube(size=[POWER_X + T, POWER_Y+T, POWER_Z + T]);
    }
    translate([-(POWER_X)/2 - T, OFFSET_Y, -(POWER_Z)]) {
        cube(size=[POWER_X + T, POWER_Y, POWER_Z]);
    }
    translate([-(POWER_X)/2 - T, OFFSET_Y, -(POWER_Z) - T/2]) {
        cube(size=[POWER_X + T, POWER_Y/2, POWER_Z]);
    }
    
    
}
