FACES 				= 180;
EPSILON 			= 0.1;

// Marks - absolute location.
M_WAY_BACK          = -40;
M_POMMEL            = -37;      // approximate. hard to measure.
M_SPEAKER_BACK      = -4;
M_0                 = 0;        // physical back of the lightsaber main body.
M_AFT_STOP          = 10.0;
M_DISPLAY           = 12.0;
DZ_DISPLAY          = 33;
M_DISPLAY_FRONT     = M_DISPLAY + DZ_DISPLAY;

M_CRYSTAL_SECTION   = 83.6;
M_1                 = 116.3;    // mark on the saber case used for the CNC cut; used to compute other marks. 
                                // in a completely different coordinate system
M_2                 = 116.3 + 84.2; // full length of center section

DZ_CRYSTAL_SECTION  = 19.0;
DZ_CRYSTAL_BUFFER   = 3.0;

M_CRYSTAL_VIEW_START = M_CRYSTAL_SECTION + DZ_CRYSTAL_BUFFER;
M_CRYSTAL_VIEW_END = M_CRYSTAL_SECTION + DZ_CRYSTAL_SECTION - DZ_CRYSTAL_BUFFER;
M_CRYSTAL_VIEW_CENTER = (M_CRYSTAL_VIEW_END + M_CRYSTAL_VIEW_START)/2;
DZ_CRYSTAL_VIEW = M_CRYSTAL_VIEW_END - M_CRYSTAL_VIEW_START;

// These values are from the CNC
M_INSET_START       = M_1 + (63.1/2) - 20;
DZ_INSET            = 40.0;
M_INSET_END         = M_INSET_START + DZ_INSET;
DX_INSET            = 18.5;

M_PORT_CENTER       = M_INSET_START + 8.0;
M_BOLT              = M_INSET_START + 20.0;
M_SWITCH_CENTER     = M_INSET_START + 29.0;
M_USB               = M_INSET_START + 35.0;

M_EMITTER_BASE      = M_INSET_END + 4.0;

H_FAR               = 240;

D_INNER      		= 32.258;
R_INNER             = D_INNER / 2;
D_THREAD            = 33.8; // FIXME
D_OUTER             = 37.6;
R_OUTER             = D_OUTER / 2;
D_AFT               = 25.8;
R_AFT               = D_AFT / 2;

H_BUTTRESS			= 4.0;  // > 3.2 so parts can be machined in

// fixme
W_CRYSTAL           = 12.1;
H_CRYSTAL           = 10;
Y_CRYSTAL           = 9;        // offset up of the crystal
Z_CRYSTAL           = 52;

X_DOTSTAR           = 5.0 + 0.5;  // from spec
Y_DOTSTAR           = 1.4;  // from spec
DOTSTAR_STRIP       = 12.4;
