M_TRANSITION		= 114;
M_LED_HOLDER_FRONT	= M_TRANSITION + 44.5;
M_LED_HOLDER_BACK	= M_LED_HOLDER_FRONT - 10;
M_SWITCH_CENTER		= M_TRANSITION + 25.5;
M_DOTSTAR			= M_TRANSITION + 12;

/*
// Celestia
M_PORT_CENTER       = M_TRANSITION + 6;
X_SWITCH			= 0.3;
*/

// Luna
M_PORT_CENTER       = M_TRANSITION + 5.5; // Luna
X_SWITCH			= 0.0;

M_SPKR_RING			= 15.5;		// Front of the speaker ring.
M_RAIL_START        = 12;
M_POMMEL_FRONT      = 10;       // Where the pommel becomes aft
M_POMMEL_BACK       = 9.0;      // Back of the pommel, leaving (in this case)
                                // enough space for the various pommels.
M_0                 = 0;

H_BUTTRESS			= 4;

M_BUTTRESS_0		= M_SPKR_RING + H_BUTTRESS;			// speaker holder, behind wings
M_BUTTRESS_3		= M_SPKR_RING + H_BUTTRESS + 55;
M_BUTTRESS_4		= M_SPKR_RING + 70;		// holds speaker in place, space for foam

M_WAY_BACK			= -20;
H_FAR				= 200;

MM_TO_INCHES = 1 / 25.4;
INCHES_TO_MM = 25.4;

D_FORWARD			= 25.4;
R_FORWARD 			= D_FORWARD / 2;
D_AFT				= 32.5;
R_AFT 				= D_AFT / 2;
T_TRANSITION_RING	= 4;

Y_SWITCH			= 9.6;
D_SWITCH			= 12.5;
D_SWITCH_TOP		= 14.5;
D_PORT 				= 12.7;		// FIXME
D_SMALL_PORT		= 7.9;

D_SABER_OUTER		= 37.5;

X_DOTSTAR           = 5.0;  // from spec
Y_DOTSTAR           = 1.4;  // from spec
DOTSTAR_SPACE		= 7;

H_BATTERY           = 68;
BATTERY_BIAS        = 0.5;  // downward bias to fit it tape / protector
D_BATTERY           = 18.50 + 0.5;    // An 1850. Huh. 
R_BATTERY			= D_BATTERY / 2;

H_SPKR_PLASTIC 		= 2.3;
D_SPKR_PLASTIC 		= 28 + 1;
SPKR_OFFSET         = 1.5;    // distance from FRONT of pommel to speaker
D_SPKR_INNER 		= D_SPKR_PLASTIC - 1.5;

D_POMMEL			= 31.0; // 31.5 is correct, but snug
R_POMMEL 			= D_POMMEL / 2;
D_AFT_RING			= 33.5;

RAIL_ANGLE_0		=  25; //38;
RAIL_ANGLE_1		= -18; //-38;
RAIL_ANGLE_2		=  180 - RAIL_ANGLE_0;
RAIL_ANGLE_3		= -180 - RAIL_ANGLE_1;

X_RAIL				= 3;
Y_RAIL				= 4;

W_WING  			= 25;
H_WING  			= 10.5;	// higher over power converter

W_MC 				= 18;
MC_RAIL             = 2.8;
RAIL_DY             = -13;
H_MC    			= 10;
Y_MC    			= -15.5;	// low config -13.5, this value in flat config
Z_MC_35 			= 71.5;		// includes SD
SHOULDER_DX			= 2.5;
SHOULDER_DZ			= 3.5;

HORN_WIDTH			= 5;
HORN_BASE_WIDTH		= 6.5;

BEAM_WIDTH          = 5.0;
BEAM_HEIGHT         = 8.0;

M_RF24				= 95;	// fixme
RF24_PUNCH_X0		= 10;
RF24_PUNCH_X1		= 15;
RF24_PUNCH_Y		= 1;	// fixme
RF24_PUNCH_DY		= 8.61;
RF24_T				= 3;	// fixme
RF24_OUTER_T		= 5;

// Battery
// 68mm
//   - 4 front holder
//   - 4 rear holder
//   - 2 slop
// 58mm, 2.28in

