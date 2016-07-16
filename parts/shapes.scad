FACES = 180;
EPSILON = 0.1;

module tube(_h, _rInner, _rOuter) {
	difference() {
		cylinder(h=_h, r=_rOuter, $fn=FACES);
		translate([0,0,-EPSILON]) {
			cylinder(h=_h + EPSILON*2, r=_rInner, $fn=FACES);
		}
	}
}

module shelf(_r0, _h, _r1) {
	rotate_extrude($fn=FACES) {
		polygon([[_r0, 0], [_r1, _h], [_r0, _h]]);
	}
}
