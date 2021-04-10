use <../threads.scad>

H = 6.0 + 1.5;
D_PLATE = 7.8;

D_BASE = 4.8;
H_BASE = 2.4;
D_PIN = 1.9;
D_TACTILE = 3.5;
H_TACTILE = 3.0;

D_RIM = 10.0;
H_RIM = 2.0;

$fn = 40;
//$fa = 0.2;
//$fs = 1.0;


difference() 
{
    union() {
        cylinder(h=H, d=D_PLATE);
        cylinder(h=H_RIM, d=D_RIM);
    }
    // top part
    translate([0, 0, H_TACTILE]) 
        english_thread (0.164, 32, (H - H_TACTILE + 0.1)/25.4);
    // pin
    cylinder(h=H, d=D_PIN);

    // switch
    cylinder(h=H_TACTILE, d=D_TACTILE);
}