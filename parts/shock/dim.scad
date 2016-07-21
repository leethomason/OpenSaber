FACES = 180;
EPSILON = 0.1;
VERSION_MAJOR = 1;
VERSION_MINOR = 0;

INCHES_TO_MM = 25.4;
MM_TO_INCHES = 1 / INCHES_TO_MM;

// inner diameter of the lightsaber tube.
// shock is constant, within tolerances
D_INNER      		= 32.258;    // 1.225 -> 1.250 -> 1.270
R_INNER = D_INNER / 2;

// dimensions of the main ring.
H_RING = 16;
T_RING = 2.5;

// dimensions of pin holders and pins
PIN	   = 2.54;
H_PIN_HOLDER = 8;
T_PIN_HOLDER_WALL = 2;

// Pin counts:
X_EMITTER = 4;
Y_EMITTER = 1;


H_BUTTRESS			= 0.175 * INCHES_TO_MM;
D_BATTERY			= 0.732 * INCHES_TO_MM;

W_PCB				= 0.70 * INCHES_TO_MM;
H_PCB				= 0.08 * INCHES_TO_MM;

W_OUTER_DISPLAY		= 0.91 * INCHES_TO_MM;
W_INNER_DISPLAY		= 0.47 * INCHES_TO_MM;
H_OUTER_DISPLAY		= 0.08 * INCHES_TO_MM;
H_INNER_DISPLAY		= 0.14 * INCHES_TO_MM;

D_SWITCH			= 12.5;						// actually 12, by thread.

D_ROD				= 0.160 * INCHES_TO_MM;
ROD_GAP 			= 0.050 * INCHES_TO_MM;

OFFSET_EMITTER = -PIN + sqrt(R_INNER * R_INNER - PIN * PIN * Y_EMITTER * Y_EMITTER);

module emitterPin(h)
{
    translate([-X_EMITTER * PIN / 2, OFFSET_EMITTER, 0]) {
        cube([X_EMITTER * PIN, PIN, h]);
    }	
}

