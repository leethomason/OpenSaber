use <../threads.scad>
use <../shapes.scad>
include <dim.scad>

PORT_SCREW_PAD      = 3;
D_PORT              = 12;
PORT_LEN            = D_PORT + PORT_SCREW_PAD * 2;
H_PORT              = 8;
H_CYLINDER          = 3.0;
D_CYLINDER          = 14;
EPS                 = 0.1;
OFFSET              = R_INNER - H_PORT;
H_SHELF             = (D_CYLINDER - 0.5 * 25.4) / 2;
PARTIAL_THREAD      = false;

//intersection() 
{
    difference() {
        tube(PORT_LEN, D_INNER/2 - T_RING, D_INNER/2);

        translate([OFFSET-EPS, 0, PORT_SCREW_PAD + D_PORT/2]) {
            rotate([0,90,0]) {
                BOLT_LEN = H_PORT + EPS*2;
                if (false) {
                    english_thread(diameter=0.5, length=BOLT_LEN * MM_TO_INCHES, threads_per_inch=28, internal=true);
                }
                else {
                    cylinder(h=BOLT_LEN, r=D_PORT/2, $fn=FACES);
                }
                if (PARTIAL_THREAD) {                
                    cylinder(h=H_CYLINDER, r1=D_CYLINDER/2, r2=(0.5 * 25.4)/2, $fn=FACES);
                }
            }
        }
    }    
}

