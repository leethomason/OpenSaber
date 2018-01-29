include <dim.scad>

EPS = 0.01;
EPS2 = EPS * 2;

module battery() {
    color("yellow") translate([0, DY_BATTERY, 0]) {
        cylinder(d=D_BATTERY, h = Z_BATTERY + EPS2);
    }
}

module switch()
{
    D_OUTER_TOP = 13.8;
    D_INNER_TOP = 11.0;
    H_TOP       =  1.5;
    H_BODY      = 13;   // approx. connections below.

    color("yellow") {
        rotate([-90, 0, 0]) {
            translate([0, 0, Y_SWITCH]) {
                cylinder(h=5, d=D_OUTER_TOP+1);
                translate([0, 0, -H_BODY]) {
                    cylinder(h = H_BODY, d=D_SWITCH);
                }            
            }
        }
    }
}


module mc(cutoutLow=false)
{
    color("aqua") {
        // lower
        translate([-X_MC/2, 0, 0])
            cube(size=[X_MC, 5.5, 61.5]);
        // Under space
        SOLDER = 2.2;
        UNDER = cutoutLow ? 20 : 3;
        //translate([-X_MC/2 + SOLDER, -UNDER, 0])
        //    cube(size=[X_MC - SOLDER*2, UNDER, 61.5]);
        // sd
        //translate([-6, -2, -3.6])
        //    cube(size=[12, 7.5, 3.6]);
        // shield
        translate([-X_MC/2, 5.5, 14])
            cube(size=[X_MC, Y_MC - 5.5, 53]);
        // usb
        //translate([-6, -4, 61.5])
        //    cube(size=[12, 9.5, 1.5]);

    }
}

cylinder(h=1, d=D_INNER);
translate([0, 0, M_POMMEL]) {
    translate([0, D_INNER/2 - D_BATTERY/2, 0]) battery();
    translate([0, -12, 0]) mc();
}
translate([0, 0, M_SWITCH_CENTER]) {
    switch();
}