// Varies; size of bolt hole in top.
// D_BASE = 3.9; // standard bolt
//D_BASE = 1.8; // the little pins
D_BASE = 4.2; // bad cub

// Varies; height of base ring.
//H_RIM = 4.0; //4.5 too hig; // 3.5;
H_RIM = 3.5;

// Varies; height of entire thing
//H = 6.5; // with raised header
H = 8.0; // with flat top

SQUARE_OFF = true;

D_PLATE = 7.8;
D_TACTILE = 3.5;
H_TACTILE = 3.0;
D_PIN = 1.9;  
H_BASE = 2.4;

D_RIM = 10.0;

$fn = 80;

difference() 
{
    union() {
        cylinder(h=H, d=D_PLATE);
        cylinder(h=H_RIM, d=D_RIM);
    }
    // top part
    translate([0, 0, H - H_BASE]) 
        cylinder(h=H_BASE, d=D_BASE);

    // pin
    cylinder(h=H, d=D_PIN);

    // switch
    cylinder(h=H_TACTILE, d=D_TACTILE);
    H_CONE = (D_TACTILE - D_PIN)/2;
    translate([0, 0, H_TACTILE]) 
        cylinder(h=H_CONE, d1=D_TACTILE, d2=D_PIN);

    if (SQUARE_OFF) {
        translate([-10, 3.5, 0]) cube(size=[20, 20, 20]);
    }
}