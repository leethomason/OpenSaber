/*
    Classic ultrasabers poor design:
    - aft with ~1.25 inch bore
    - middle section with 0.9" bore
    - forward with 1" bore, but not the correct length to secure the heatsink
    - uses a header coupler for the heatsink
*/

HAS_FORE_AFT = true;

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
M_SWITCH_CENTER = 130.08 + 17.0s * 0.5; // center of ORIGINAL switch

DZ_PLATE = 36.0;
W_PLATE = 19.0;
M_PLATE = M_SWITCH_CENTER - DZ_PLATE / 2;
DY_FLAT = 1.5;

PLATE_PAD = 2.5;
M_BOLT = M_SWITCH_CENTER + 1.0;
M_PORT = M_SWITCH_CENTER - DZ_PLATE / 2 + PLATE_PAD + D_PLATE_PORT / 2;
M_SWITCH = M_SWITCH_CENTER + DZ_PLATE / 2 - PLATE_PAD - D_PLATE_SWITCH / 2;

M_HEAT_SINK = DZ_TOTAL - DZ_BLADE_HOLDER - DZ_HEAT_SINK;

KEYJOINT = 8;
KEYJOINT_T = 5.0;
