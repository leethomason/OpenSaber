$fn = 80;

D = 25.4;
//H = 4.0; // normal
H = 1.5; // shim

R_HOLE = 8.92;
D_HOLE = 4.4;

difference() {
    cylinder(h=H, d=D);
    for(r=[0:5]) {
        rotate([0, 0, r*60]) translate([R_HOLE, 0, 0]) cylinder(h=H, d=D_HOLE);
    }
    cylinder(h=H, d=12);
}