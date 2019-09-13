FACES 				= 180;
EPSILON 			= 0.1;

// Marks - absolute location.
M_WAY_BACK          = -40;
M_POMMEL            = -37;      // approximate. hard to measure.
//M_SPEAKER_BACK      = -36;
M_0                 = 0;        // physical back of the lightsaber main body.
M_AFT_STOP          = 10.5;     // fixme, was 10
M_DISPLAY           = 10.5;     // fixme
DZ_DISPLAY          = 33;
M_DISPLAY_FRONT     = M_DISPLAY + DZ_DISPLAY;

M_CRYSTAL_SECTION = 83.6;
DZ_CRYSTAL_SECTION = 19.0;
DZ_CRYSTAL_BUFFER = 3.0;

M_CRYSTAL_VIEW_START = M_CRYSTAL_SECTION + DZ_CRYSTAL_BUFFER;
M_CRYSTAL_VIEW_END = M_CRYSTAL_SECTION + DZ_CRYSTAL_SECTION - DZ_CRYSTAL_BUFFER;
M_CRYSTAL_VIEW_CENTER = (M_CRYSTAL_VIEW_END + M_CRYSTAL_VIEW_START)/2;
DZ_CRYSTAL_VIEW = M_CRYSTAL_VIEW_END - M_CRYSTAL_VIEW_START;

M_PORT_CENTER       = 132.0;    // fixme
M_SWITCH_CENTER     = 153;      // fixme 147.5;
M_USB               = 158;

M_INSET_START       = 125.0;
M_INSET_END         = 162.0;
M_INSET_CENTER = (M_INSET_START + M_INSET_END) / 2;

M_EMITTER_BASE      = 165.5;

H_FAR               = 240;

// inner diameter of the lightsaber tube.
// shock is constant, within tolerances
D_INNER      		= 32.258;
R_INNER             = D_INNER / 2;
D_OUTER             = 37.6;
D_AFT               = 25.8;
R_AFT               = D_AFT / 2;
//D_AFT_RING          = 33.5;
//H_AFT_RING          = 6;
//H_AFT_LIP           = 4;
//FLOOR_Y             = -12;
//Y_FLATTEN           = -15;

// emitter head
//D_EMITTER_RING      = 33.4;

// dimensions of the main ring.
//H_RING 				= 16;
//T_RING 				= 2.5;

// dimensions of pin holders and pins
//PIN	   				= 2.54;
//H_PIN_HOLDER 		= 8;
//T_PIN_HOLDER_WALL 	= 2;

//N_TEETH				= 2;
//TEETH_ANGLE			= 90;

H_BUTTRESS			= 4.0;  // > 3.2 so parts can be machined in

// fixme
W_CRYSTAL           = 12.1;
H_CRYSTAL           = 10;
Y_CRYSTAL           = 9;        // offset up of the crystal
Z_CRYSTAL           = 52;

X_DOTSTAR           = 5.0 + 0.5;  // from spec
Y_DOTSTAR           = 1.4;  // from spec
DOTSTAR_STRIP       = 12.4;

DX_USB              = 21.0;
DX_USB_INNER        = 14.0;
DY_USB              = 10.4;
SLOT_USB            = 1.0;
