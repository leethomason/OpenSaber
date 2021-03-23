HAS_FORE_AFT = true;
HAS_COUPLER = 2;    // 0: none, 1: header, 2: thread
HAS_PIN_IGNITION = false;
HAS_KEYJOINT_PILLAR = false;
HAS_SIDE_VENTS = true;
ALLOW_POWER_NUT_TO_TURN = false;

D_OUTER = 37.55;
D_AFT_INNER = 32.5;
D_FORE_INNER = 25.0;
DZ_AFT = 113.7;
DZ_TOTAL = 112.1 + 128.0;
DZ_BLADE_HOLDER = 25.4 * 1.8;
DZ_HEAT_SINK = 26;

D_INIT_SWITCH = 17.0;
DZ_SPEAKER = 12.0;
D_PLATE_PORT = 11.0;
D_PLATE_SWITCH = 8.4;

M_0 = 0.0;
M_POMMEL = 9.5;
M_MC = M_POMMEL;
M_FORE = DZ_AFT;
M_SWITCH_CENTER = 130.08 + 17.0 * 0.5; // center of ORIGINAL switch

DZ_PLATE = 36.0;
W_PLATE = 17.0;
M_PLATE = M_SWITCH_CENTER - DZ_PLATE / 2;
DY_FLAT = 1.5;
NUM_VENTS = 3;

PLATE_PAD = 2.5;
M_BOLT = M_SWITCH_CENTER + 1.0;
M_PORT = M_SWITCH_CENTER - DZ_PLATE / 2 + PLATE_PAD + D_PLATE_PORT / 2;
M_SWITCH = M_SWITCH_CENTER + DZ_PLATE / 2 - PLATE_PAD - D_PLATE_SWITCH / 2;
M_COUPLER_START = M_SWITCH + 6.5;

KEYJOINT = 8;
KEYJOINT_T = 5.0;
