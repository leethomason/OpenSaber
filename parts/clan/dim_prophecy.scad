HAS_FORE_AFT = true;            // has 2 different bores
HAS_COUPLER = 3;                // 0: none, 1: header, 2: thread, 3: short thread
HAS_PIN_IGNITION = false;        // if true, small pin, else proper switch
HAS_KEYJOINT_PILLAR = false;    // reinforces the key joint - small sabers
HAS_SIDE_VENTS = true;          // side vents on front part
ALLOW_POWER_NUT_TO_TURN = false;
COVERTEC_CUTOUT = 1;

D_OUTER = 37.14;
D_AFT_INNER = 32.5 - 0.1;
D_FORE_INNER = 25.4 - 0.2;
DZ_AFT = 101.4;
DZ_TOTAL = 101.0 + 100; // just defines inner()

DZ_SPEAKER = 12.0;
D_PLATE_PORT = 11.0;

M_0 = 0.0;
M_MC = -11.0;
M_FORE = DZ_AFT;
M_SWITCH_CENTER = 139.2 + 16.0/2.0; // center of ORIGINAL switch

DZ_PLATE = 33.0;
W_PLATE = 17.0;
M_PLATE = M_SWITCH_CENTER - DZ_PLATE / 2;
DY_FLAT = 1.5;
NUM_VENTS = 6;

PLATE_PAD = 1.5;
M_BOLT = M_SWITCH_CENTER + 1.0;
M_PORT = M_SWITCH_CENTER - DZ_PLATE / 2 + PLATE_PAD + D_PLATE_PORT / 2;
M_SWITCH = M_SWITCH_CENTER + DZ_PLATE / 2 - PLATE_PAD - 8.2 / 2;
M_COUPLER_START = M_SWITCH + 6.0;

KEYJOINT = 8;
KEYJOINT_T = 5.0;
