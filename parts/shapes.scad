FACES = 180;
EPSILON = 0.1;

module tube(p_h, rOuter, rInner) {
	difference() {
		cylinder(h=p_h, r=rOuter, $fn=FACES);
		translate([0,0,-EPSILON]) {
			cylinder(h=p_h + EPSILON*2, r=rInner, $fn=FACES);
		}
	}
}

/*
needs a 2016 version.
module tubeArc(deg, p_h, rOuter, rInner) {
	//rotate([90,0,0]) {
		rotate_extrude(angle=30) {
			translate([rInner,0,0]) {
				square(size=[rOuter-rInner, p_h], center=false);
			}
		}
	//}
}
*/
