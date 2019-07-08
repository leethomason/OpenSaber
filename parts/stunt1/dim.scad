// Tuned per build. Measurement is from the back
// of the chassis, after the pommel. 

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

// Vented
/*
DZ_AFT = 115.0;
DZ_FORE = 0.0;
D_AFT = 28.5;
D_FORE = 28.5;
M_SWITCH = 115.0;   // 0 for no switch
BATTERY_TYPE = "18500";

ADD_EMITTER = false;    // adds extra length to hold dynamic heat sink
USE_AA = false;          // use an AA battery holder (vs 18650)
ACCESS_PCB = false;     // makes the PCB switches accessible from the pommel
*/


DZ_TOTAL = DZ_AFT + DZ_FORE;    // excluding emitter
DZ_PCB = 30;

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
