H = 6.0 + 1.5;
D_PLATE = 7.8;

D_BASE = 4.8;
H_BASE = 2.4;
D_PIN = 1.9;
D_TACTILE = 3.5;
H_TACTILE = 3.0;

D_RIM = 10.0;
H_RIM = 1.5;

$fn = 80;

difference() 
{
    union() {
        cylinder(h=H, d=D_PLATE);
        cylinder(h=H_RIM, d=D_RIM);
    }
    // top part
    translate([0, 0, H - H_BASE]) cylinder(h=H_BASE, d=D_BASE);
    // pin
    cylinder(h=H, d=D_PIN);

    // switch
    cylinder(h=H_TACTILE, d=D_TACTILE);
    H_CONE = (D_TACTILE - D_PIN)/2;
    translate([0, 0, H_TACTILE]) cylinder(h=H_CONE, d1=D_TACTILE, d2=D_PIN);

    translate([-50, -50, -1]) cube(size=[100, 50, 50]);
}