include <dim.scad>

module tube(p_h, rOuter, rInner) {
	difference() {
		cylinder(h=p_h, r=rOuter, $fn=FACES);
		translate([0,0,-EPSILON]) {
			cylinder(h=p_h + EPSILON*2, r=rInner, $fn=FACES);
		}
	}
}