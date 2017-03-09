M_TRANSITION		= 101;
M_LED_HOLDER_FRONT	= M_TRANSITION + 62;
M_LED_HOLDER_BACK	= M_LED_HOLDER_FRONT - 10;
M_SWITCH_CENTER		= M_TRANSITION + 45;
M_DOTSTAR			= M_TRANSITION + 33;	// FIXME center mark of first dotstar
M_PORT_CENTER       = M_TRANSITION + 14;

M_POMMEL_BACK		= -12.5;	// Back of the pommel
M_0					= 0;
M_POMMEL_FRONT		= 10;		// Where the pommel becomes aft
M_RAIL_START		= 12;
M_SPKR_RING			= 15.5;		// Front of the speaker ring.

H_BUTTRESS			= 4;

M_BUTTRESS_0		= M_SPKR_RING;			// speaker holder, behind wings
M_BUTTRESS_1		= M_SPKR_RING + 18;
M_BUTTRESS_2		= M_SPKR_RING + 37;
M_BUTTRESS_3		= M_BUTTRESS_0 + H_BUTTRESS + 52;
M_BUTTRESS_4		= M_SPKR_RING + 70;		// holds speaker in place, space for foam

M_WAY_BACK			= -20;
H_FAR				= 200;

MM_TO_INCHES = 1 / 25.4;
INCHES_TO_MM = 25.4;

D_FORWARD			= 25.4;
R_FORWARD 			= D_FORWARD / 2;
D_AFT				= 32.5;
R_AFT 				= D_AFT / 2;
T_TRANSITION_RING	= 3;

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
D_BATTERY           = 18.50 + 0.5;    // An 1850. Huh. 
R_BATTERY			= D_BATTERY / 2;

H_SPKR_METAL   		= 8 + 0.5;
D_SPKR_METAL   		= 23.6;	
H_SPKR_PLASTIC 		= 2.7;
D_SPKR_PLASTIC 		= 28;
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

// Battery
// 68mm
//   - 4 front holder
//   - 4 rear holder
//   - 2 slop
// 58mm, 2.28in

