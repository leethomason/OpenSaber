FACES 				= 180;
EPSILON 			= 0.1;

INCHES_TO_MM 		= 25.4;
MM_TO_INCHES 		= 1 / INCHES_TO_MM;

// inner diameter of the lightsaber tube.
// shock is constant, within tolerances
D_INNER      		= 32.258;    	// 1.225 -> 1.250 -> 1.270
R_INNER 			= D_INNER / 2;
D_AFT               = 25.8;
R_AFT               = D_AFT / 2;

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

DISPLAY_W           = 23;
DISPLAY_L           = 32;
DISPLAY_MOUNT_W     = 17;
DISPLAY_MOUNT_L     = 26;
D_DISPLAY_MOUNT     = 2;

D_SWITCH			= 12.5;						// actually 12, by thread.

D_ROD				= 0.160 * INCHES_TO_MM;
ROD_GAP 			= 0.050 * INCHES_TO_MM;

H_BATTERY           = 68;
D_BATTERY           = 18.50 + 0.5;    // An 1850. Huh. 

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
