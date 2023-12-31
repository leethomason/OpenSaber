M_0 = 0.0;
M_POMMEL_OUTER = -12.5;
M_BACK = 10.0 - 16.5 + 1.5;
M_SECTION_1 = 85.0 + 92.5;
M_SECTION_2 = M_SECTION_1 + 63.3;
M_SECTION_3 = M_SECTION_2 + 50.5;
DZ_BODY = 7.0 * 25.4;

M_FORE = M_0 + DZ_BODY;
M_STOP = M_FORE - 1.0;  // measure at +1; give a little space

DZ_SPEAKER = 10.0;
DZ_BOARDS = 56.0 + 18.0 + 4.0;

M_MC_BATTERY = M_BACK + DZ_SPEAKER;
DZ_BATTERY = 65.0 + 4.0;
M_PORT_RING = M_MC_BATTERY + DZ_BOARDS;
DZ_PORT = 18.0;
DZ_PORT_OFFSET = 10.0;
M_BOLT_RING = M_PORT_RING + DZ_PORT;
M_TACTILE_RING = M_BOLT_RING + 10.0;
M_RING_END = M_TACTILE_RING + 13.0;
M_END = M_RING_END + 2.0;               // very end of the carriage

M_PORT = M_PORT_RING + DZ_PORT_OFFSET;
M_BOLT = M_BOLT_RING + 5.0;
M_SWITCH = M_TACTILE_RING + 6.0;

N_BATT_BAFFLES = 10;

M_FIRST_RING = 17.5;
DZ_RING = 2.5;
DZ_RING_SPACE = 14.0;
D_RING = 35.8;
N_RINGS = 8;

D_OUTER = 37.0;
D0 = 31.60;

DZ_CRYSTAL = 40.0;
DX_CRYSTAL = 9.0;
DY_CRYSTAL = 5.0;

DZ_THREAD = 15.0;

DZ_BUBBLE = 15.4;
DX_BUBBLE = 9.0;

AL_STOCK_3 = 3.3;
AL_STOCK_6 = 6.6;

M_CHAMBER = M_END;
DZ_CHAMBER_BASE_PLATE = AL_STOCK_3 * 2;

DZ_BRASS = 44.0;
DZ_BRASS_OFFSET = AL_STOCK_3;
M_BRASS_START = M_CHAMBER + DZ_BRASS_OFFSET;
M_BRASS_END = M_BRASS_START + DZ_BRASS;
M_CHAMBER_END = M_BRASS_END + DZ_BRASS_OFFSET;

SMOOTH_SECTION = 58.5;
SMOOTH_INSET = 15.0;
DZ_WINDOW = SMOOTH_SECTION - SMOOTH_INSET * 2;
M_WINDOW = M_FORE - SMOOTH_SECTION + SMOOTH_INSET;
M_WINDOW_END = M_WINDOW + DZ_WINDOW;

N_WINDOW = 6;
DX_WINDOW = 8.0;

// ANCHOR_ROD: threaded, holds the crystal in place
A_ANCHOR_ROD = 0;
D_ANCHOR_ROD = 3.6;
R_ANCHOR_ROD = 9.0;
DEPTH_HEAD_ANCHOR = 3.0;
D_HEAD_ANCHOR = 6.5;

// TUBE_ROD: hollow tube, holds the piece apart, runs wires through
A_TUBE_ROD = 60;
DO_TUBE_ROD = 4.7;
DI_TUBE_ROD = 3.5;
DEPTH_TUBE_ROD = AL_STOCK_3;
R_TUBE_ROD = 12.0;

// BOLT_ROD: threaded, holds the piece together
A_BOLT_ROD = 120;
D_BOLT_ROD = 3.6;
R_BOLT_ROD = 11.0;
DEPTH_HEAD_BOLT = AL_STOCK_3;
D_HEAD_BOLT = 6.5;

D_ROD_NUT = 9.0;
DEPTH_ROD_NUT = 4.5;    // with offset for lock washer
T_TOP_RING = AL_STOCK_3;
M_TOP_RING = M_BRASS_END - T_TOP_RING;
T_BOLT_RING = AL_STOCK_3;

M_MC = M_MC_BATTERY + 2.0;
M_MC_END = M_MC + 2.2 * 25.4;
M_BUBBLE_PCB_END = M_PORT_RING;
M_BUBBLE_PCB = M_BUBBLE_PCB_END - 0.7 * 25.4;
DX_PCB = 0.7 * 25.4;

M_BUBBLE = M_BUBBLE_PCB + 0.35 * 25.4 - DZ_BUBBLE / 2.0;
M_BUBBLE_END = M_BUBBLE_PCB + 0.35 * 25.4 + DZ_BUBBLE / 2.0;

PLATE_EDGE = 2.0;
D_SWITCH_PLATE_HOLE = 11.0;
DX_SWITCH_PLATE = 16.0;    // 17?

M_SWITCH_PLATE_START = M_PORT - D_SWITCH_PLATE_HOLE / 2 - PLATE_EDGE;
M_SWITCH_PLATE_END = M_SWITCH + 6.0;
DZ_SWITCH_PLATE = M_SWITCH_PLATE_END - M_SWITCH_PLATE_START;
DY_SWITCH_PLATE = 5.0;
