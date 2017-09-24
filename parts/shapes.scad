FACES   = 180;
EPSILON = 0.1;
PIN	    = 2.54;


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


module pinsAtTube(r, h, nX, nY, longY)
{
	OFFSET = sqrt(r * r - PIN * PIN * nX * nX / 4);

	Y = longY ? 40 : PIN * nY;

    translate([-nX * PIN / 2, OFFSET - PIN * nY, 0]) {
        cube([nX * PIN, Y, h]);
    }	
}

module roundedRect(h, d)
{
	$fn=20;

	D0 = d * 0.8;
	D1 = d * 0.2;
	H = h;

	hull()
	{
		translate([-D0/2, -D0/2, 0]) {
			cube([D0, D0, H]);
		}
		translate([-D0/2, -D0/2, 0])  cylinder(d=D1, h=H);
		translate([ D0/2, -D0/2, 0])  cylinder(d=D1, h=H);
		translate([-D0/2,  D0/2, 0])  cylinder(d=D1, h=H);
		translate([ D0/2,  D0/2, 0])  cylinder(d=D1, h=H);
	}
}

module cubePair(x, size)
{
	translate([x, 0, 0])
		cube(size=size);
	mirror([1,0,0]) translate([x, 0, 0])
		cube(size=size);
}
