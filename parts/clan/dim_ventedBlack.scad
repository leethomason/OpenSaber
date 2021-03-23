/*
    Cylidrical section for electronics.
    Seperate heatsink
*/

HAS_FORE_AFT = false;
HAS_COUPLER = 0;    // 0: none, 1: header, 2: thread
HAS_PIN_IGNITION = true;
HAS_KEYJOINT_PILLAR = true;
HAS_SIDE_VENTS = false;
ALLOW_POWER_NUT_TO_TURN = true;

D_OUTER = 34.3;
D_AFT_INNER = 28.5;
D_FORE_INNER = 19.0;
DZ_AFT = 98.3;
DZ_TOTAL = 158.0;
DZ_BLADE_HOLDER = 0;    // separate component
DZ_HEAT_SINK = 0;

DZ_SPEAKER = 12.0;
D_PLATE_PORT = 11.0;
D_PLATE_SWITCH = 8.4;
NUM_VENTS = 3;

M_0 = 0.0;
M_POMMEL = 12.8;
M_MC = M_POMMEL;
M_FORE = DZ_AFT;
M_SWITCH_CENTER = 115.0;  // center of ORIGINAL switch. in this case the center line.

// --- used in topcut
DZ_PLATE = 31.0;
W_PLATE = 16.0;
// ----

M_PLATE = M_SWITCH_CENTER - DZ_PLATE / 2;
DY_FLAT = 1.5;

PLATE_PAD = 2.5;

// Packed! Eek.
M_BOLT = M_SWITCH_CENTER + 2.0;     // w=8.0 mm
M_PORT = M_BOLT - 4.0 - 6.3 - 0.1; // 12.6 mm
M_SWITCH = M_BOLT + 4.0 + 4.1 + 0.1; // 8.2 mm
M_COUPLER_START = M_SWITCH + 6.0;

KEYJOINT = 8;
KEYJOINT_T = 5.0;
