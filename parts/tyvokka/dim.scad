M_0                 = 0;            // physical back of the lightsaber main body.
M_SPEAKER_BACK      = 10;           // FIXME
M_BATTERY_BACK      = 25;           // FIXME
M_BATTERY_FRONT     = 100;
M_CAPSULE_BACK      = 110;          // FIXME
M_CAPSULE_START     = M_CAPSULE_BACK - 5;
M_EMITTER           = 165.0;        // FIXME

DZ_BAFFLE           = 4.0;

D_THREAD            = 33.5;
D_INNER      		= 32.258;
R_INNER             = D_INNER / 2;  
D_OUTER             = 37.9;             // at the switch

/*
    d=16.2 cut; outer-outer 42.5 (measured), 42.0 nominal.
    use nominal for hole positioning
*/
D_CAPSULE = 16.2;
DZ_CAPSULE_NOM  = 42.0;
DZ_CENTER = (DZ_CAPSULE_NOM - D_CAPSULE) / 2;

DZ_PORT         = -DZ_CENTER;
DZ_SWITCH       = DZ_CENTER;