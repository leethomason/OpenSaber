D_INNER = 26.0 - 0.1;
DZ_INNER = 180.9;

MC_X                =   0.7 * 25.4;
MC_Y                =   9.0;
MC_Z                =   2.2 * 25.4;

M_0 = 0.0;
M_MC = 7.0;
M_BATTERY = M_MC + MC_Z;

M_SW_0 = 134.2;             // back edge, not center. Hard to measure. The rest is standard.
M_SW_1 = M_SW_0 + 30.1;     // front edge, not center
D_SW_0 = 12.0;
D_SW_1 = 10.0;

M_BOLT = M_SW_0 - 8.0 - 3.0/2;
M_BOLT_1 = M_BOLT + 51.8 - 1.5;

M_END = 160.0;  // very back of the heatsink. guesstimate
M_SWITCH = M_SW_0 + D_SW_0 / 2;
M_PORT = M_SW_1 - D_SW_1 / 2;

DZ_BATTERY = 52.4;
D_BATTERY = 18.4;

N_BAFFLES = 14;
DZ_BUTTRESS = (DZ_BATTERY + MC_Z) / (N_BAFFLES * 2);

T = 4.0;