D_INNER = 26.0 - 0.1;
DZ_INNER = 180.9;

MC_X                =   0.7 * 25.4;
MC_Y                =   9.0;
MC_Z                =   2.2 * 25.4;

M_0 = 0.0;
M_MC = 8.0;
M_BATTERY = M_MC + MC_Z;

M_BOLT = 125.0;
M_END = 160.0;  // very back of the heatsink. guesstimate
M_SWITCH = 141.8;
M_PORT = 159.0;

DZ_BATTERY = 52.4;
D_BATTERY = 18.4;

N_BAFFLES = 20;
DZ_BUTTRESS = (DZ_BATTERY + MC_Z) / (N_BAFFLES * 2);

T = 4.0;