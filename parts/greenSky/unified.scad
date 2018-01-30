include <dim.scad>
use <../shapes.scad>

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

module rod() {
    $fn = 30;
    translate([13, 0, 0])
        cylinder(h=200, d=D_ROD);
}

module rods() {
    rod();
    mirror([1, 0, 0]) rod();
}

module halfBridge()
{
    polygonYZ(1.5, [[0,0], [H_BUTTRESS*2, H_BUTTRESS*2], [H_BUTTRESS*2, 0]]);
}

module baffle(battery=true, mc=true, useRods=true, bridge=true, h=H_BUTTRESS)
{
    TROUGH_0 = 10;
    TROUGH_1 = 14;
    difference() {
        cylinder(h=h, d=D_INNER);
        if (battery)
            translate([0, D_INNER/2 - D_BATTERY/2, -EPS]) battery();
        if (mc)
            translate([0, DY_MC, -EPS]) mc();
        translate([-TROUGH_0/2, 0, -EPS]) 
            cube(size=[TROUGH_0, 30, h + EPS2]);
        translate([-TROUGH_1/2, -5, -EPS]) 
            cube(size=[TROUGH_1, 5, h + EPS2]);
        translate([-X_MC/2 + 2, -20, -EPS])
            cube(size=[X_MC - 4, 15, h + EPS2]);
        if (useRods)
            rods();
    }
    if (bridge) {
        translate([X_MC/2+1, -11, h]) halfBridge();        
        mirror([1, 0, 0]) translate([X_MC/2+1, -11, h]) halfBridge();
    }
}

// Parts
translate([0, 0, M_POMMEL]) {
    translate([0, D_INNER/2 - D_BATTERY/2, 0]) battery();
    translate([0, DY_MC, 0]) mc();
}
translate([0, 0, M_SWITCH_CENTER]) {
    switch();
}

translate([0, DY_CRYSTAL, M_CRYSTAL]) crystal();
translate([0, -5.5, M_PORT_CENTER]) port();


// Case
translate([0, 0, M_POMMEL]) baffle(h=5);

NBAF = 8;
for(x=[0:NBAF-1])
     translate([0, 0, M_POMMEL + H_BUTTRESS*2 + H_BUTTRESS*2*x]) 
        baffle(bridge = x<NBAF-1);
