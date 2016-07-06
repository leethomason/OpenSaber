use <../threads.scad>
use <../shapes.scad>
include <dim.scad>

PORT_SCREW_PAD      = 3;
PORT_LEN            = D_PORT_SCREW + PORT_SCREW_PAD * 2;
H_PORT              = 6;
H_CYLINDER          = 3.0;
D_CYLINDER          = 14;
EPS                 = 0.1;
OFFSET              = R_AFT - H_PORT;
H_SHELF             = (D_CYLINDER - 0.5 * 25.4) / 2;
PARTIAL_THREAD      = false;

//intersection() 
{
    difference() {
        intersection() {
            cylinder(h=PORT_LEN, r=R_AFT, $fn=FACES);
            translate([OFFSET,-100,0]) {
                cube(size=[200, 200, 200]);
            }
        }

        translate([OFFSET-EPS, 0, PORT_SCREW_PAD + R_PORT_SCREW]) {
            rotate([0,90,0]) {
                BOLT_LEN = H_PORT + EPS*2;
                if (true) {
                    english_thread(diameter=0.5, length=BOLT_LEN * MM_TO_INCHES, threads_per_inch=28, internal=true);
                }
                else {
                    cylinder(h=BOLT_LEN, r=D_PORT_SCREW/2, $fn=FACES);
                }
                if (PARTIAL_THREAD) {                
                    cylinder(h=H_CYLINDER, r1=D_CYLINDER/2, r2=(0.5 * 25.4)/2, $fn=FACES);

                    //translate([0, 0, H_CYLINDER - H_SHELF]) {
                    //    shelf(D_CYLINDER / 2, H_SHELF, 0.5 * 25.4 / 2);
                    //}
                }
            }
        }
    }    
    //cube([100,100,100]);   
}

