//M_TRANSITION		= 114;
//M_LED_HOLDER_FRONT	= M_TRANSITION + 48;
//M_LED_HOLDER_BACK	= M_LED_HOLDER_FRONT - 10;
//M_SWITCH_CENTER		= M_TRANSITION + 30.5;
//M_FORWARD_PCB       = M_TRANSITION + 20;
//M_PORT_CENTER       = M_TRANSITION + 14;
M_CHAMBER           = 122;      // FIXME
                                // End of the plastic carriage, beginning
                                // of the metal chamber.

M_SPKR_RING			= 15.5;		// Front of the speaker ring.
M_RAIL_START        = 12;
M_POMMEL_FRONT      = 10;       // Where the pommel becomes aft
M_POMMEL_BACK       = -5.0;
M_0                 = 0;

H_BUTTRESS			= 4;

M_BUTTRESS_0		= M_SPKR_RING + H_BUTTRESS;			// speaker holder, behind wings
M_BUTTRESS_3		= M_SPKR_RING + H_BUTTRESS + 59;
M_BUTTRESS_4		= M_SPKR_RING + 70;		// holds speaker in place, space for foam

M_WAY_BACK			= -20;
H_FAR				= 200;

MM_TO_INCHES = 1 / 25.4;
INCHES_TO_MM = 25.4;

D_AFT				= 32;
R_AFT 				= D_AFT / 2;
T_TRANSITION_RING	= 4;

Y_SWITCH			= 9.6;
D_SWITCH			= 12.5;
D_SWITCH_TOP		= 14.5;
D_PORT 				= 12.7;		// FIXME
D_SMALL_PORT		= 7.9;

D_SABER_OUTER		= 37.5;

H_BATTERY           = 68;
BATTERY_BIAS        = 0.5;  // downward bias to fit it tape / protector
D_BATTERY           = 18.50 + 0.5;    // An 1850. Huh. 
R_BATTERY			= D_BATTERY / 2;

H_SPKR_PLASTIC 		= 2.3;
D_SPKR_PLASTIC 		= 28 + 1;
SPKR_OFFSET         = 1.5;    // distance from FRONT of pommel to speaker
D_SPKR_INNER 		= D_SPKR_PLASTIC - 1.5;

// FIXME
D_POMMEL			= 29.0;
R_POMMEL 			= D_POMMEL / 2;
D_AFT_RING			= 33.5;

H_SPKR_METAL        = 8 + 0.5;
D_SPKR_METAL        = 23.6; 
H_SPKR_PLASTIC      = 2.4;
D_SPKR_PLASTIC      = 28;

X_RAIL				= 3;
Y_RAIL				= 4;

W_WING  			= 25;
H_WING  			= 10.5;	// higher over power converter

W_MC 				= 18;
MC_RAIL             = 2.8;
RAIL_DY             = -13;

// FIXME correct MC values and the circuit() function
H_MC    			= 10;
Y_MC    			= -15.5;	// low config -13.5, this value in flat config
Z_MC_35 			= 71.5;		// includes SD

SHOULDER_DX			= 2.5;
SHOULDER_DZ			= 3.5;

HORN_WIDTH			= 5;
HORN_BASE_WIDTH		= 6.5;

BEAM_WIDTH          = 5.0;
BEAM_HEIGHT         = 8.0;

POWER_X = 11;
POWER_Y = 14.5;
POWER_Z = 10;
