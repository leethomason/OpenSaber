M0 = 0.0;       // start of speaker. not the usual place.
M1 = 107.6;
M2 = M1 + 10.1;
M_AL = M2 - 50.8;
M_SET_PIN = M_AL + 24.5;

D0 = 31.7;
D1 = 29.0;

// See: Z_BATTERY_18650 = 65 + 4;
DZ_BATTERY = 65 + 4;

DZ_PCB = 0.975 * 25.4;
DY_PCB_TOP = 10.0; // FIXME
DY_PCB_BOTTOM = DY_PCB_TOP - 1.6;

// 5, 10, 15mm offsets
DY_PILLAR_TOP = DY_PCB_BOTTOM - 10.0;
DX_PCB = 0.90 * 25.4;

DZ_SPKR_HOLDER = 8.5;

M_MC = M0 + DZ_SPKR_HOLDER;
M_PCB = M_MC + DZ_BATTERY;
M_FORE_PLATE = M_PCB + DZ_PCB;

DZ_AFT = M1 - M0;