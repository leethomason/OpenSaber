// Tuned per build. Measurement is from the back
// of the chassis, after the pommel. 

// Graflex
DZ_AFT = 107 - 15;
DZ_FORE = 0.0;
D_AFT = 32.5;
M_STEPDOWN = 0;
D_FORE = 33.5;
M_SWITCH = 0;
D_RING = 33.5;
DZ_RING = 4.0;
BATTERY_TYPE = "18500";
EMITTER = "none";
ACCESS_PCB = true;
DZ_PCB = 15;


/*
// Vader
DZ_AFT = 176.0 - 2.5;
DZ_FORE = 0.0;
D_AFT = 32.5;
M_STEPDOWN = 80.0;
D_FORE = 32.0;
M_SWITCH = 122.0 - 10.0 + 2.3;
D_RING = 33.5;
DZ_RING = 4.0;
BATTERY_TYPE = "18500";

EMITTER = "closed";    // adds extra length to hold dynamic heat sink
ACCESS_PCB = true;     // makes the PCB switches accessible from the pommel
DZ_PCB = 30;
*/


DZ_TOTAL = DZ_AFT + DZ_FORE;    // excluding emitter

H_BUTTRESS			= 4;
R_AFT 				= D_AFT / 2;
R_FORE           = D_FORE / 2;

Z_OFFSET = 3;

DX_POWER = 11;
DY_POWER = 9;
DZ_POWER = 14;

DY_PCB = 0;

JOINT_DZ = 8;
JOINT_T = 3;
