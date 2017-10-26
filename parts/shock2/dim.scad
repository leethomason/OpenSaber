FACES 				= 180;
EPSILON 			= 0.1;

INCHES_TO_MM 		= 25.4;
MM_TO_INCHES 		= 1 / INCHES_TO_MM;

// Marks - absolute location.
M_WAY_BACK          = -40;
M_POMMEL            = -37;    // approximate. hard to measure.
M_SPEAKER_BACK      = -36;
M_0                 = 0;        // physical back of the lightsaber main body.
M_AFT_STOP          = 10;
M_DISPLAY           = 14;       // was 15
M_PORT_CENTER       = 57.5;
M_CRYSTAL_START     = 68.5;
M_MC                = 68;
M_CRYSTAL_VIEW_CENTER = 93;
M_SWITCH_CENTER     = 145;
M_EMITTER_BASE      = 185;

M_PORT_BUTTRESS     = 63.5;
M_FRONT_BUTTRESS    = 104.0;

H_FAR               = 240;

// inner diameter of the lightsaber tube.
// shock is constant, within tolerances
D_INNER      		= 32.258;
R_INNER             = D_INNER / 2;
D_INNER_CORE        = 0.5 + 0.800 * INCHES_TO_MM;
D_OUTER             = 37.6;
D_AFT               = 25.8;
R_AFT               = D_AFT / 2;
D_AFT_RING          = 33.5;
H_AFT_RING          = 6;
H_AFT_LIP           = 4;
Y_FLATTEN           = -15;


H_SPEAKER = 3.6 + 1.0;  // space for the physical speaker
X_SPEAKER = 20;
Y_SPEAKER = 14.2;   
Y_SPEAKER_INNER = 8.4;  // air channel
H_SPEAKER_HOLDER = H_SPEAKER + 1.2;

// emitter head
D_EMITTER_RING      = 33.4;

// dimensions of the main ring.
H_RING 				= 16;
T_RING 				= 2.5;

// dimensions of pin holders and pins
PIN	   				= 2.54;
H_PIN_HOLDER 		= 8;
T_PIN_HOLDER_WALL 	= 2;

N_TEETH				= 2;
TEETH_ANGLE			= 90;

// Pin counts:
X_EMITTER 			= 4;
Y_EMITTER 			= 1;

H_BUTTRESS			= 3;       // internal buttress / baffles

W_CRYSTAL           = 12.1;
H_CRYSTAL           = 9;
Y_CRYSTAL           = 8;        // offset up of the crystal
Z_CRYSTAL           = 50;

DISPLAY_W           = 23;
DISPLAY_L           = 32;
DISPLAY_MOUNT_W     = 17;
DISPLAY_MOUNT_L     = 26;
D_DISPLAY_MOUNT     = 2;
Y_DISPLAY           = 9.5;
DISPLAY_PINS        = 20;

D_SWITCH			= 12.5;						// actually 12, by thread.
Y_SWITCH            = 13.5;

D_PORT              = 7.9;

W_MC                = 18;
H_MC                = 10;
Z_MC_35             = 71.5;     // includes SD
SHOULDER_DX         = 2.5;
SHOULDER_DZ         = 3.5;

X_ROD               = 10.5;
Y_ROD               =  3.5;
D_ROD               =  3.4;

POWER_DX            = 9.6;
POWER_DY            = 14;
POWER_DZ            = 11;
POWER_Y             = 0.5;
POWER_X             = 0;

H_BATTERY           = 68;
D_BATTERY           = 18.50 + 0.5;    // An 1850. Huh. 
BATTERY_DROP        = 0.5;

X_DOTSTAR           = 5.0;  // from spec
Y_DOTSTAR           = 1.4;  // from spec

OFFSET_EMITTER = -PIN + sqrt(R_INNER * R_INNER - PIN * PIN * X_EMITTER * X_EMITTER / 4);

module emitterPin(h, longY)
{
	Y = longY ? 40 : PIN;

    translate([-X_EMITTER * PIN / 2, OFFSET_EMITTER, 0]) {
        cube([X_EMITTER * PIN, Y, h]);
    }	
}

module emitterHolder(h)
{
    translate([-X_EMITTER * PIN / 2, OFFSET_EMITTER, 0]) {
    	translate([-T_PIN_HOLDER_WALL, -T_PIN_HOLDER_WALL, 0]) {
      		cube([X_EMITTER * PIN + T_PIN_HOLDER_WALL*2, PIN + T_PIN_HOLDER_WALL*2, h]);
    	}
    }	
	
}

// Uses the same dimensions as the emitter holder so they 
// can be used interchangeably.
module lock(h)
{
    translate([-X_EMITTER * PIN / 2 - T_PIN_HOLDER_WALL, -20, 0]) {
		cube([X_EMITTER * PIN + T_PIN_HOLDER_WALL * 2, 40, h]);
	}	
}

