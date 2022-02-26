// Varies; size of bolt hole in top.
// D_BASE = 3.9; // standard bolt
//D_BASE = 1.8; // the little pins
D_BASE = 4.2;

// Varies; height of base ring.
//H_RIM = 4.0; 
//H_RIM = 3.5;
// H_RIM = 3.3;
H_RIM = 3.0;

// Varies; height of entire thing
//H = 6.5; // with raised header
H = 8.0; // with flat top

SQUARE_OFF = true;
SHORTEN_FRONT = false;

D_PLATE = 7.8;
D_TACTILE = 3.8;
H_TACTILE = 3.0;
D_PIN = 3.8;  
H_BASE = 2.4;

D_RIM = 10.0;
SQUARE = SHORTEN_FRONT ? 3.5 : D_RIM/2;

$fn = 80;

difference() 
{
    union() {
        cylinder(h=H, d=D_PLATE);
        if (SQUARE_OFF) {
            R = 1.0;
            hull() {
                translate([D_RIM/2-R, SQUARE-R, 0]) cylinder(h=H_RIM, r=R);
                translate([D_RIM/2-R, -D_RIM/2+R, 0]) cylinder(h=H_RIM, r=R);
                translate([-D_RIM/2+R, SQUARE-R, 0]) cylinder(h=H_RIM, r=R);
                translate([-D_RIM/2+R, -D_RIM/2+R, 0]) cylinder(h=H_RIM, r=R);
            }
        }
        else {
            cylinder(h=H_RIM, d=D_RIM);
        }
    }
    // top part
    translate([0, 0, H - H_BASE]) 
        cylinder(h=H_BASE, d=D_BASE);

    // pin
    //cylinder(h=H, d=D_PIN);

    // switch
    cylinder(h=H_TACTILE, d=D_TACTILE);
    H_CONE = (D_TACTILE - D_PIN)/2;
    translate([0, 0, H_TACTILE]) 
        cylinder(h=H_CONE, d1=D_TACTILE, d2=D_PIN);

    if (SHORTEN_FRONT) {
        translate([-10, SQUARE, 0]) cube(size=[20, 20, 20]);
    }
}