include <commonUnified.scad>

$fn = 80;

*powerPortRing(30, 4, 22, 10);
*translate([0, 0, 50]) switchRing(30, 3, 30, 15);
*translate([0, 0, -40]) speakerHolder(24, 20, 4);
*translate([0, 0, 100]) oneBaffle(32, 4);
     
/*
forwardAdvanced(25.4,   // diameter
                50.0,  // dz
                5.0, 33.0,    // overlap
                10.0,
                28.0
            );
*/

//emitterHolder(32);
//emitterBase(32);

baffleMCBattery(32, 10, 4, bridgeStyle=3);