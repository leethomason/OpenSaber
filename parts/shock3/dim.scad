FACES 				= 180;
EPSILON 			= 0.1;

// Marks - absolute location.
M_WAY_BACK          = -40;
M_POMMEL            = -37;      // approximate. hard to measure.
M_SPEAKER_BACK      = -3;       // can go pretty far. challenge is fitting on printer bed.
M_0                 = 0;        // physical back of the lightsaber main body.
M_AFT_STOP          = 10.0;
M_DISPLAY           = 12.0;
DZ_DISPLAY          = 33;
M_DISPLAY_FRONT     = M_DISPLAY + DZ_DISPLAY;

M_CRYSTAL_START     = 70.0 - 10.0 - 2.5;
M_CRYSTAL_END       = 100.0 + 10.0 + 0.5;
M_1                 = 116.3;    // mark on the saber case used for the CNC cut; used to compute other marks. 
                                // in a completely different coordinate system
M_2                 = 116.3 + 84.2; // full length of center section

DZ_CRYSTAL_SECTION  = M_CRYSTAL_END - M_CRYSTAL_START;

// These values are from the CNC
M_INSET_START       = M_1 + (63.1/2) - 20;
DZ_INSET            = 40.0;
M_INSET_END         = M_INSET_START + DZ_INSET;
DX_INSET            = 18.5;

WINDOW_0_START      = 71.1;
WINDOW_0_DZ         =  7.27;
WINDOW_1_START      = 87.1;
WINDOW_1_DZ         = 12.75;

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
H_CRYSTAL           = 8;
Y_CRYSTAL           = 11;        // offset up of the crystal

X_DOTSTAR           = 5.0 + 0.5;  // from spec
Y_DOTSTAR           = 1.4;  // from spec
DOTSTAR_STRIP       = 12.4;

STOCK_H = 6.35;     // 1/4 inch aluminum

BUTTON_WELL = 4.5;      // adjusted, depending. was: 3.5
BUTTON_HEAD_D = 7.2;
BUTTON_HEAD_H = 1.6;
BUTTON_THREAD_H = 4.0;
BUTTON_THREAD_D = 4.0;
BUTTON_UNDER_WELL = 4.0;
BUTTON_UNDER_WELL_D = 10.0;

TACTILE_POST_H = 5.5;
TACTILE_POST_D = 3.4;
TACTILE_BASE_H = 3.8;
TACTILE_BASE_X = 6.0;

M_METAL_ART = M_AFT_STOP + H_BUTTRESS*13;
DZ_METAL_ART = H_BUTTRESS*11;
Y_METAL_ART = 9;

