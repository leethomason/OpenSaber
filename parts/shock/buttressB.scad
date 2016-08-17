/*
    The back of the emitter.
    Holds the power port.
 */

include <buttress.scad>

POWER_X = 11;
POWER_Y = 4;
POWER_Z = 9;
T = 2;
OFFSET_Y = D_INNER / 2 - POWER_Y - 1.9;

LED_X = 13;
LED_Y = 8;
LED_Z = 1.5;

CABLE_X = 8;

difference() {
    part(crystal=true, pcb=7, biasRight=false);
    
    // LED holder
    translate([-LED_X/2, CRYSTAL_Y - LED_Y/2, H_BUTTRESS - LED_Z]) {
        cube(size=[LED_X, LED_Y, LED_Z]);
    }        

    // Cable notch
    translate([-CABLE_X/2, -1, 0]) {
        cube(size=[CABLE_X, 6, 3]);
    }
}



*cylinder(r=D_INNER/2, h=1);

// Power port holder
difference() {
    translate([-(POWER_X + T*2)/2, OFFSET_Y, -(POWER_Z + T)]) {
        cube(size=[POWER_X + T*2, POWER_Y, POWER_Z + T]);
    }
    translate([-(POWER_X)/2, OFFSET_Y, -(POWER_Z)]) {
        cube(size=[POWER_X, POWER_Y, POWER_Z]);
    }
}



