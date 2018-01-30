include <dim.scad>

$fn = 40;
EPS = 0.01;
EPS2 = EPS * 2;

module battery() {
    color("yellow") translate([0, DY_BATTERY, 0]) {
        cylinder(d=D_BATTERY, h = Z_BATTERY + EPS2);
    }
}

module switch(extend=false)
{
    D_OUTER_TOP = 13.8;
    D_INNER_TOP = 11.0;
    H_TOP       =  1.5;
    H_BODY      = 18;   // approx. connections below.

    color("yellow") {
        rotate([-90, 0, 0]) {
            translate([0, 0, Y_SWITCH]) {
                cylinder(h=H_TOP + (extend ? 5 : 0), d=D_OUTER_TOP+1);
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
        translate([-X_MC/2, 0, 0])
            cube(size=[X_MC, Y_MC, Z_MC]);
    }
}

module crystal()
{
    color("pink") scale([1.0, Y_CRYSTAL/X_CRYSTAL, 1.0]) {
        cylinder(d=X_CRYSTAL, h=Z_CRYSTAL);
    }
}

module port()
{
    rotate([-90, 0, 0]) {
        cylinder(h=21, d=D_PORT);
        cylinder(h=15, d=11.3);
    }
}

cylinder(h=1, d=D_INNER);
translate([0, 0, M_POMMEL]) {
    translate([0, D_INNER/2 - D_BATTERY/2, 0]) battery();
    translate([0, -12.5, 0]) mc();
}
translate([0, 0, M_SWITCH_CENTER]) {
    switch();
}
translate([0, DY_CRYSTAL, M_CRYSTAL]) crystal();
translate([0, -5.5, M_PORT_CENTER]) port();