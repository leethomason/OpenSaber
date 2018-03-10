$fn = 90;

H = 10;
D = 26;

difference() {
    union() {
        cylinder(h=H, d=D);	
        cylinder(h=4, d=D+4);
    }
	cylinder(h=H, d=8);
	for(r=[0:5]) {
		rotate([0, 0, 60*r]) {
			//translate([D/2, 0, 0,]) cylinder(h=H, d=8);
            translate([8, 0, 0]) cylinder(h=H, d=4);
            translate([8, -2, 0]) cube(size=[10, 4, H]);
                
		}
	}
}

