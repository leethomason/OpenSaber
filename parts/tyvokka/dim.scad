M_0                 = 0;            // physical back of the lightsaber main body.
M_SPEAKER_BACK      = 10;           // could probably go to 9
M_BATTERY_BACK      = 25;           // FIXME
M_BATTERY_FRONT     = 100;          // FIXME
M_CAPSULE_BACK      = 105.0;
M_EMITTER           = 161.0;        // FIXME
M_PHYSICAL_STOP     = 200.0;

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

M_CAPSULE_CENTER = M_CAPSULE_BACK + DZ_CAPSULE_NOM / 2;

DZ_PORT         = -DZ_CENTER;
DZ_SWITCH       = DZ_CENTER;