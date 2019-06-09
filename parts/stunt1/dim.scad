// Tuned per build. Measurement is from the back
// of the chassis, after the pommel. 

// Vented
DZ_AFT = 115.0;
DZ_FORE = 0.0;
D_AFT = 28.5;
D_FORE = 28.5;
M_SWITCH = 115.0;   // 0 for no switch
ADD_EMITTER = false; // adds extra length
USE_AA = true;

// Test
/*
DZ_AFT = 90;
DZ_FORE = 29;
D_AFT = 32.0;
D_FORE = 28.0; //25.4;
M_SWITCH = 105.0;   // 0 for no switch
ADD_EMITTER = true; // adds extra length
*/

DZ_TOTAL = DZ_AFT + DZ_FORE;    // excluding emitter
DZ_PCB = 27;
DZ_HOLDER = 30;    // fixme 

H_BUTTRESS			= 4;
R_AFT 				= D_AFT / 2;
R_FORE           = D_FORE / 2;

Z_OFFSET = 3;

DX_POWER = 11;
DY_POWER = 9;
DZ_POWER = 14;

DY_PCB = 0;
ROT = -25;
Y_TWEAK = -5;

JOINT_DZ = 8;
JOINT_T = 3;
