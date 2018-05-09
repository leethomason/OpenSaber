include <commonUnified.scad>

$fn = 80;

*powerPortRing(30, 4, 22, 10);
*translate([0, 0, 50]) switchRing(30, 3, 30, 15);
*translate([0, 0, -40]) speakerHolder(24, 20, 4);
*translate([0, 0, 100]) 
    oneBaffle(32, 4);
     
/*
n = nBafflesNeeded(4);
baffleMCBattery(32, n, 4, 34, 6);
//battery(32);
*/

//oneBaffle(32, 4);

//oledDisplay();
//oledHolder(35, 4, 50, 8);

//translate([0, 0, 0]) columnY(4, 20, 4);

//pcb(15, 25, 12, 20, 4);

//pcbHolder(32, 4, 30, 5, 10, [15, 20, 20], [12, 0, 16]);

//bridge(40, 10, 4);
//bridge(10, 40, 4);
bridge(20, 20, 4);