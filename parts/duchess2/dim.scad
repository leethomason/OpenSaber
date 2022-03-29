use <../commonUnified.scad>
M_0 = 0;

// Test trims - rather tight.
M_START = -(41.5 - 0.40*25.4);
M_MC = -19.0;
DZ_SPKR = M_MC - M_START;
M_MC_BATTERY = M_START + DZ_SPKR;

DZ_BODY = 101.6; // MHS part
M_AFT_FRONT = DZ_BODY - 10.5 - 0.5; //
M_TRANSITION_RING = DZ_BODY - 15.0;
DZ_AFT = M_AFT_FRONT - M_START;

D_VENT_OUTER = 36.8;
D_VENT_INNER = 25.8;

D_INNER = 31.60;    // 31.7 is fine for the body, but too tight for the pommel
D_OUTER = 36.9;

DZ_BAFFLE = 3.0;

N_BATT_BAFFLES = nBafflesNeeded(DZ_BAFFLE, "18650");
M_JOINT = M_MC_BATTERY + zLenOfBaffles(N_BATT_BAFFLES, DZ_BAFFLE);

DZ_POWER_RING = 18.0;
DZ_BOLT = 8.0;          // switch to allen head for space?
DZ_SWITCH = (M_AFT_FRONT - M_JOINT - DZ_POWER_RING - DZ_BOLT);

M_BOLT_START = M_JOINT + DZ_POWER_RING;
M_SWITCH_START = M_BOLT_START + DZ_BOLT;

M_BOLT = M_BOLT_START + DZ_BOLT / 2;
M_PORT = M_JOINT + DZ_POWER_RING / 2;
M_SWITCH = M_SWITCH_START + DZ_SWITCH / 2;

INSET_W = 14.0;
D_RING = 33.4;

DX_SWITCH_PLATE = 16.5;

D_ROD = 3.4;
D_ROD_NUT = 7.0; // space for a nut to thread...
D_TUBE = 4.5;    // outer radius of tube that carries power to emitter
D_TUBE_INNER = 3.2; // FIXME
D_CRYSTAL_SPACE = 12.0;

HEAD_HOLDER_SETBACK = 4.0;  // how deep threads go into pillars
A_BOLT_0 = 90;              // angle of bolt
A_BOLT_1 = 270;             // angle of bolt
ANGLE_OFFSET = 33;          // from rod to tube

CHAMBER_RING_DZ = 1.6 * 2 + 3.0; // guess - thickness of brass, working space inside
