INNER_DX = 21.5;
INNER_DY = 20.7;
INNER_ARCY = 32.0;

POWER_DX = 9.3;
POWER_DY = 14.3;
POWER_DZ = 10.7;
POWER_DZ_PIN = 4.5;

SWITCH_DX = 6.5;
SWITCH_DY = 3.5;
SWITCH_DZ = 3.5;
SWITCH_ARM = 3.0;
SWITCH_ARM_DZ = 1.0 + 0.5;

D_HOLE = 14.0;
D_CHARGER_PLUG = 11.0;

D_BOLT = 3.1;
D_SWITCH = 14.1;

// Back plate
// Speaker
// Header
// PCB
// back of plastic / start of carriage

DZ_SPKR_PLATE = 7.7;
DZ_SPKR_AND_HEADER = 25.0;

M_0 = 0.0;
//M_SPKR = 29.5;      // guess
M_CARRIAGE = DZ_SPKR_PLATE + DZ_SPKR_AND_HEADER;
M_BOLT_0 = 63.6 + D_BOLT / 2;
M_BOLT_1 = M_BOLT_0 + 91.2 + D_BOLT;
M_SWITCH = M_BOLT_1 + D_BOLT/2 + 16.1 + D_SWITCH/2;
M_BOLT_2 = M_BOLT_1 + 40.2 + D_BOLT;
M_CARRIAGE_FRONT = M_BOLT_2 + 5.0; // guess
M_BOLT_3 = M_BOLT_2 + 14.7 + D_BOLT;

M_SWITCH_SECTION_START = M_BOLT_1 - 5.0;    // guess
M_SWITCH_SECTION_END = M_BOLT_2 + 5.0;      // guess

DZ_CARRIAGE = M_CARRIAGE_FRONT - M_CARRIAGE;
DZ_SWITCH_SECTION = M_SWITCH_SECTION_END - M_SWITCH_SECTION_START;

MIN_POWER_PIN_Z = M_SWITCH - D_SWITCH/2 + D_CHARGER_PLUG/2;
ACTUAL_POWER_PIN_Z = MIN_POWER_PIN_Z;

