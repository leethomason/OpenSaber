HAS_FORE_AFT = true;            // has 2 different bores
HAS_COUPLER = 2;                // 0: none, 1: header, 2: thread
HAS_PIN_IGNITION = true;        // if true, small pin, else proper switch
HAS_KEYJOINT_PILLAR = false;    // reinforces the key joint - small sabers
HAS_SIDE_VENTS = true;          // side vents on front part
ALLOW_POWER_NUT_TO_TURN = false;

D_OUTER = 37.40;
D_AFT_INNER = 32.5;
D_FORE_INNER = 25.4;
DZ_AFT = 102.0;
DZ_TOTAL = 102.0 + 63.0;
DZ_BLADE_HOLDER = 25.4 * 2.0;
DZ_HEAT_SINK = 26;

DZ_SPEAKER = 12.0;
D_PLATE_PORT = 11.0;
D_PLATE_SWITCH = 8.4;

M_0 = 0.0;
M_POMMEL = 10.0;
M_MC = M_POMMEL;
M_FORE = DZ_AFT;
M_SWITCH_CENTER = 146.0; // center of ORIGINAL switch

DZ_PLATE = 33.0;
W_PLATE = 19.0;
M_PLATE = M_SWITCH_CENTER - DZ_PLATE / 2;
DY_FLAT = 1.5;

PLATE_PAD = 1.5;
M_BOLT = M_SWITCH_CENTER + 1.0;
M_PORT = M_SWITCH_CENTER - DZ_PLATE / 2 + PLATE_PAD + D_PLATE_PORT / 2;
M_SWITCH = M_SWITCH_CENTER + DZ_PLATE / 2 - PLATE_PAD - D_PLATE_SWITCH / 2;
M_COUPLER_START = M_SWITCH + 8.0;

KEYJOINT = 8;
KEYJOINT_T = 5.0;
