M0 = 0.0;       // start of speaker. not the usual place.
M1 = 107.6;
D0 = 31.7;
D1 = 29.0;

// See: Z_BATTERY_18650 = 65 + 4;
DZ_BATTERY = 65 + 4;
DZ_PCB = 26.0;  // fixme

DZ_SPKR_HOLDER = 8.5;
M_MC = M0 + DZ_SPKR_HOLDER;
M_PCB = M_MC + DZ_BATTERY;
M_FORE_PLATE = M_PCB + DZ_PCB;

DZ_AFT = M1 - M0;