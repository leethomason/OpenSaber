M_0                 = 0;            // physical back of the lightsaber main body.

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
DZ_CAPSULE_ACT  = 42.5;
DZ_CENTER = (DZ_CAPSULE_NOM - D_CAPSULE) / 2;

DZ_PORT         = -DZ_CENTER;
DZ_SWITCH       = DZ_CENTER;