FACES 				= 180;
EPSILON 			= 0.1;

INCHES_TO_MM 		= 25.4;
MM_TO_INCHES 		= 1 / INCHES_TO_MM;

// Marks - absolute location.
M_WAY_BACK          = -40;
M_POMMEL            = -37.5;    // approximate. hard to measure.
M_SPEAKER_BACK      = -36.5;
M_0                 = 0;        // physical back of the lightsaber main body.
M_AFT_STOP          = 10;
M_DISPLAY           = 15;       
M_PORT_CENTER       = 57.8;
M_FRONT_BUTTRESS    = 106.0;


H_FAR               = 200;

// inner diameter of the lightsaber tube.
// shock is constant, within tolerances
D_INNER      		= 32.258;    	// 1.225 -> 1.250 -> 1.270
R_INNER 			= D_INNER / 2;
D_AFT               = 25.8;
R_AFT               = D_AFT / 2;
D_AFT_RING          = 33.5;
H_AFT_RING          = 6;
H_AFT_LIP           = 4;

H_SPEAKER = 3.6 + 0.2;  // space for the physical speaker
X_SPEAKER = 20;
Y_SPEAKER = 14.2;   
Y_SPEAKER_INNER = 8.4;  // air channel
H_SPEAKER_HOLDER = H_SPEAKER + 2;

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
W_PCB				= 18;      // This is the pcb width
H_PCB				= 6;       // But the height varies a *lot*
W_CRYSTAL           = 12.1;
H_CRYSTAL           = 9; //8.2;
D_CRYSTAL_TIP       = 5.5;
H_CRYSTAL_HOLDER    = 8.0;
CRYSTAL_Y           = 8;        // offset up of the crystal

DISPLAY_W           = 23;
DISPLAY_L           = 32;
DISPLAY_MOUNT_W     = 17;
DISPLAY_MOUNT_L     = 26;
D_DISPLAY_MOUNT     = 2;
Y_DISPLAY           = 9.5;

D_SWITCH			= 12.5;						// actually 12, by thread.

X_ROD               = 10.5;
Y_ROD               =  3.5;
D_ROD               =  3.4;

H_BATTERY           = 68;
D_BATTERY           = 18.50 + 0.5;    // An 1850. Huh. 
BATTERY_DROP        = 1;

Z_NOTCH             = H_BUTTRESS / 2;
NOTCH_DEPTH         = 2;
NOTCH_WIDTH         = 3;
RAIL_WIDTH          = 4;
RAIL_EPS            = 0.5;
NOTCH_ANGLE_0       = 195;
NOTCH_ANGLE_1       = -20;

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

