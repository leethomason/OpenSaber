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
    H = extend ? H_TOP + 5 : H_TOP;

    color("yellow") {
        rotate([-90, 0, 0]) {
            translate([0, 0, Y_SWITCH]) {
                cylinder(h=H, d=D_OUTER_TOP+1);
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
    translate([0, DY_CRYSTAL, 0])
    color("pink") scale([1.0, Y_CRYSTAL/X_CRYSTAL, 1.0]) {
        cylinder(d=X_CRYSTAL, h=Z_CRYSTAL);
    }
}

module port(extend=false)
{
    H = extend ? 30 : 21;
    rotate([-90, 0, 0]) {
        cylinder(h=H, d=D_PORT);
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

module cBaffle() {
    difference() {
        translate([0, DY_CRYSTAL, 0])
            cylinder(h=H_CRYSTAL_BAFFLE, d=D_CRYSTAL_BAFFLE);
        crystal();
        translate([0, DY_CRYSTAL, -EPS]) rotate([0, 0, -135])
            cube(size=[20, 20, 20]);
    }
}

// Parts
*translate([0, 0, M_POMMEL]) {
    translate([0, D_INNER/2 - D_BATTERY/2, 0]) battery();
    translate([0, DY_MC, 0]) mc();
}
*translate([0, 0, M_SWITCH_CENTER]) switch();
*translate([0, 0, M_CRYSTAL]) crystal();
*translate([0, -5.5, M_PORT_CENTER]) port(extend=true);


// Case
translate([0, 0, M_POMMEL]) baffle(h=5);

NBAF = 8;
for(x=[0:NBAF-1])
     translate([0, 0, M_POMMEL + H_BUTTRESS*2*(1 + x)]) 
        baffle(bridge = x<NBAF-1);

M_END_BAFFLE = M_POMMEL + H_BUTTRESS*2*(1 + NBAF);

DCB = M_CRYSTAL - M_END_BAFFLE;
PORT_FRONT = DCB + M_END_BAFFLE;

// Power port
difference() {
    translate([0, 0, M_END_BAFFLE]) union() {
        tube(h=DCB, inner=(D_INNER-2)/2, outer=D_INNER/2);
        intersection() {
            cylinder(h=DCB, d=D_INNER);
            translate([-20, D_INNER/2 - 6, 0])
                cube(size=[40, 10, DCB]);
        }        
    }
    translate([0, -5.5, M_PORT_CENTER]) port(true);
}

// crystal
for(z=[0:7])
    translate([0, 0, M_CRYSTAL + z*2*H_CRYSTAL_BAFFLE])
        cBaffle();

SWITCH_FRONT = M_SWITCH_CENTER + 8;

// Cover
translate([0, 0, PORT_FRONT]) difference() {
    tube(h=M_CRYSTAL + Z_CRYSTAL - PORT_FRONT, inner=(D_INNER-2)/2, outer=D_INNER/2);
    scale([0.5, 1.0, 1.0])
        rotate([0, 0, 225])
            cube(size=[40, 40, M_CRYSTAL + Z_CRYSTAL - PORT_FRONT]);        
}

// Switch
difference() {
    Z = M_CRYSTAL + Z_CRYSTAL;
    DZ = SWITCH_FRONT - Z;

    translate([0, 0, Z]) union() {
        tube(h=DZ, inner=(D_INNER-2)/2, outer=D_INNER/2);
        intersection() {
            cylinder(h=DZ, d=D_INNER);
            translate([-20, D_INNER/2 - 6, 0])
                cube(size=[40, 10, DZ]);
        }        
    }
    translate([0, 0, M_SWITCH_CENTER]) switch(true);
}

// Emitter
intersection() {
    cylinder(h=200, d=D_INNER);
    translate([0, 0, SWITCH_FRONT])
        for(r=[0:19])
            rotate([0, 0, r*18]) translate([-1, 0, 0])
            polygonYZ(2, [[2, 0], [5,4], [6,2], [12, 4], [D_INNER/2,0]]);
}
