/*
    Emitter buttress.
 */

include <buttress.scad>

//part(crystal="tip", pcb=5, biasRight=false, lowerWiring=true);    // 1
//part(crystal="body", pcb=12, biasRight=true, upperWiring=true);  // 2 x2
part(crystal="body", pcb=12, biasRight=true, upperWiring=true, crystalHolder=true); // 3

