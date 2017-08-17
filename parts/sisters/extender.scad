$fn=90;

H = 16.5;
DELTA = 24.5;

difference() {
	cylinder(d=35.5, h=H);	
	cylinder(d=14.0, h=H);
	translate([ DELTA/2, 0, 0]) cylinder(d=3.5, h=H);
	translate([-DELTA/2, 0, 0]) cylinder(d=3.5, h=H);
}
