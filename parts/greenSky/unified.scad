include <dim.scad>
use <../shapes.scad>

$fn = 90;
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
    TAPER = 5;
    translate([0, DY_CRYSTAL, 0])
    color("pink") scale([1.0, Y_CRYSTAL/X_CRYSTAL, 1.0]) {
        cylinder(d=X_CRYSTAL, h=Z_CRYSTAL - TAPER);
        translate([0, 0, Z_CRYSTAL - TAPER])
            cylinder(h=TAPER, d1 = X_CRYSTAL, d2=X_CRYSTAL *0.7);
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
    translate([13, 0, -20])
        cylinder(h=200, d=D_ROD);
}

module rods() {
    rod();
    mirror([1, 0, 0]) rod();
}

T_BRIDGE = 1.6;

module halfBridge()
{
    polygonYZ(T_BRIDGE, [[0,0], [H_BUTTRESS*2, H_BUTTRESS*2], [H_BUTTRESS*2, 0]]);
}

X_BRIDGE = X_MC/2+1;

module baffle(battery=true, mc=true, useRods=true, bridge=true, h=H_BUTTRESS, d=D_INNER)
{
    TROUGH_0 = 10;
    TROUGH_1 = 10;
    TROUGH_2 = 14; //X_MC - 4;    // not related to X_MC

    difference() {
        cylinder(h=h, d=d);
        if (battery)
            translate([0, D_INNER/2 - D_BATTERY/2, -EPS]) battery();
        if (mc)
            translate([0, DY_MC, -EPS]) mc();
        translate([-TROUGH_0/2, 0, -EPS]) 
            cube(size=[TROUGH_0, 30, h + EPS2]);
        translate([-TROUGH_1/2, -5, -EPS]) 
            cube(size=[TROUGH_1, 5, h + EPS2]);
        translate([-TROUGH_2/2, -20, -EPS])
            cube(size=[TROUGH_2, 15, h + EPS2]);
        if (useRods)
            rods();
    }
    if (bridge) {
        translate([X_BRIDGE, -11, h]) halfBridge();        
        mirror([1, 0, 0]) translate([X_BRIDGE, -11, h]) halfBridge();

        translate([X_BRIDGE, 2, h]) halfBridge();        
        mirror([1, 0, 0]) translate([X_BRIDGE, 2, h]) halfBridge();

        intersection() {
            translate([0, 0, -EPS]) cylinder(h=H_BUTTRESS*2 + EPS2, d=D_INNER);
            union() {
                translate([TROUGH_2/2, DY_MC - 5, -EPS])
                    cube(size=[TROUGH_2, 5, H_BUTTRESS*2 + EPS]);
                mirror([1,0,0]) translate([TROUGH_2/2, DY_MC - 5, -EPS])
                    cube(size=[TROUGH_2, 5, H_BUTTRESS*2 + EPS]);
            }
        }
    }
}

module cBaffle(threads=false) {
    intersection() {
        cylinder(d=D_INNER, h=H_CRYSTAL_BAFFLE*2);

        difference() {
            union() {        
                translate([0, DY_CRYSTAL, 0]) {
                    cylinder(h=H_CRYSTAL_BAFFLE, d=D_CRYSTAL_BAFFLE);
                }
                tube(h=H_CRYSTAL_BAFFLE, do=D_INNER, di=D_INNER-6);
                difference() {
                    tube(h=H_CRYSTAL_BAFFLE*2, do=D_INNER-2, di=D_INNER-4);                
                    translate([-20, -4, 0])
                        cube(size=[40, 40, 40]);
                }
            }
            translate([0, DY_CRYSTAL, -EPS]) rotate([0, 0, -135])
                cube(size=[20, 20, 20]); 
        }        
    }
}

module capsule(theta0, theta1, r=2)
{
    hull() {
        rotate([-90, -0, theta0]) cylinder(h=20, r=r);
        rotate([-90, -0, theta1]) cylinder(h=20, r=r);
    }
}

module speaker()
{
    color("yellow") {
        cylinder(h=H_SPKR_PLASTIC + EPS, d=D_SPKR_PLASTIC);
        translate([0, 0, H_SPKR_PLASTIC])
            cylinder(h=H_SPKR_METAL, d=D_SPKR_METAL);
    }
}


NUM_BAFFLES = 8;
M_END_BAFFLE = M_POMMEL + H_BUTTRESS*2*(1 + NUM_BAFFLES);

DZ_PORT = M_CRYSTAL - M_END_BAFFLE;
PORT_FRONT = DZ_PORT + M_END_BAFFLE;

SWITCH_BACK = M_SWITCH_CENTER - 9;
SWITCH_FRONT = M_SWITCH_CENTER + 9;
T_SWITCH_RING = 2;

module speakerHolder()
{
    difference() {
        translate([0, 0, M_BACK])
            tube(h=M_POMMEL - M_BACK, do=D_POMMEL, di=D_SPKR_PLASTIC - 2);
        translate([0, 0, M_SPEAKER])
            speaker();
        translate([-20, -20, M_BACK-EPS])
            cube(size=[40, 14, 20]);
        rods();

        THETA = 14;
        for(r=[0:2]) {
            rotate([0, 0, -50 + r*50]) translate([0, 0, M_BACK]) {
                translate([0, 0, 3])  capsule(-THETA, THETA, 1);
                translate([0, 0, 6]) capsule(-THETA, THETA, 1);
                translate([0, 0, 9]) capsule(-THETA, THETA, 1);
                translate([0, 0, 12]) capsule(-THETA, THETA, 1);
            }
        }
    }
}

module key(extend=false)
{
    D = 5.0 * (extend ? 1.0 : 0.98);
    DI = extend ? D_INNER + 2 : D_INNER;

    KZ0 = M_POMMEL + H_BUTTRESS*2*(NUM_BAFFLES) + H_BUTTRESS;     
    KZ  = M_POMMEL + H_BUTTRESS*2*(NUM_BAFFLES+1) + 5;
    KDZ = KZ - KZ0 + EPS;

    difference() {
        intersection() {
            cylinder(h=200, d=DI);
            union() {
                translate([X_BRIDGE, -20, KZ0])
                    cube(size=[T_BRIDGE, 40, KDZ]);
                mirror([1,0,0]) translate([X_BRIDGE, -20, KZ0])
                    cube(size=[T_BRIDGE, 40, KDZ]);
                translate([0, -20, KZ]) {
                    translate([X_BRIDGE, 0, 0]) rotate([-90, 0, 0]) cylinder(h=40, d=D);
                    translate([-X_BRIDGE, 0, 0]) rotate([-90, 0, 0]) cylinder(h=40, d=D);
                }
            }
        }
        rods();
    }
}

module aftElectronics()
{
    // Case
    translate([0, 0, M_POMMEL]) baffle(h=Z_RING, d=D_RING);

    for(x=[0:NUM_BAFFLES-1])
         translate([0, 0, M_POMMEL + H_BUTTRESS*2*(1 + x)]) 
            baffle(bridge = x < NUM_BAFFLES-1);

    key();
}

DD_HOLDER_TUBES = 4;
DZ_LED_PLATE = 3;

module ledPlate()
{
    intersection() {
        cylinder(d=D_INNER, h=200);

        difference() {
            color("aqua")  
            union() {        
                translate([0, DY_CRYSTAL, M_CRYSTAL - DZ_LED_PLATE]) {
                    cylinder(h=DZ_LED_PLATE * 0.98, d=D_CRYSTAL_BAFFLE);
                }
            }
            translate([0, DY_CRYSTAL, M_CRYSTAL - DZ_LED_PLATE]) cylinder(h=DZ_LED_PLATE+EPS, d=3.2); // LED
        }        
    }    
}

module powerPort() 
{
    DZ = 3;
    difference() {
        translate([0, 0, M_END_BAFFLE]) 
        union() {
            tube(h=DZ_PORT, di=D_INNER - DD_HOLDER_TUBES, do=D_INNER);
            intersection() {
                cylinder(h=DZ_PORT, d=D_INNER);
                union() {
                    translate([-20, D_INNER/2 - 6, 0])
                        cube(size=[40, 10, DZ_PORT]);

                    translate([6, -20, M_CRYSTAL - M_END_BAFFLE - DZ_LED_PLATE - DZ])
                        cube(size=[10, 14, DZ]);
                    mirror([1,0, 0]) translate([6, -20, M_CRYSTAL - M_END_BAFFLE - DZ_LED_PLATE - DZ])
                        cube(size=[10, 14, DZ]);
                }
            }        
        }
        translate([0, -5.5, M_PORT_CENTER]) port(true);
        rods();
        key(true);
        translate([-10, -D_INNER/2 - 1, M_END_BAFFLE]) 
            cube(size=[20, 1.3, 100]);
    }
}


module crystalHolder()
{
    difference() {
        union() {
            intersection() {
                translate([-20, -40, PORT_FRONT]) 
                    cube(size=[40, 40, M_CRYSTAL + Z_CRYSTAL - PORT_FRONT]);
                union() {
                    for(z=[0:7]) {
                        translate([0, 0, M_CRYSTAL + z*2*H_CRYSTAL_BAFFLE])
                            cBaffle();
                    }
                }    
            }
        }
        translate([0, 0, M_CRYSTAL]) crystal();
        rods();        
    }
}

module centerCover()
{
    // Cover
    translate([0, 0, PORT_FRONT]) 
    difference() {
        tube(h=M_CRYSTAL + Z_CRYSTAL - PORT_FRONT, inner=(D_INNER-2)/2, outer=D_INNER/2);
        translate([-20, -40, 0]) 
            cube(size=[40, 40, M_CRYSTAL + Z_CRYSTAL - PORT_FRONT]);

        translate([0, 0, 6])  capsule(-18, 18);
        translate([0, 0, 12]) capsule(-18, 18);
        translate([0, 0, 18]) capsule(-18, 18);
        translate([0, 0, 24]) capsule(-18, 18);

        translate([0, 0, 6])  capsule(-60, -60);
        translate([0, 0, 12])  capsule(-60, -60);
        translate([0, 0, 18])  capsule(-60, -60);

        translate([0, 0, 12])  capsule(60, 60);
        translate([0, 0, 18])  capsule(60, 60);
        translate([0, 0, 24])  capsule(60, 60);
    }
    intersection() {
        cylinder(h=200, d=D_INNER);    

        difference() {
            union() {
                translate([10, -4, PORT_FRONT + H_CRYSTAL_BAFFLE]) cube(size=[10, 30, H_CRYSTAL_BAFFLE]);
                mirror([1,0,0]) translate([10, -4, PORT_FRONT + H_CRYSTAL_BAFFLE]) cube(size=[10, 30, H_CRYSTAL_BAFFLE]);
                translate([10, -4, PORT_FRONT + H_CRYSTAL_BAFFLE*13]) cube(size=[10, 30, H_CRYSTAL_BAFFLE]);
                mirror([1,0,0]) translate([10, -4, PORT_FRONT + H_CRYSTAL_BAFFLE*13]) cube(size=[10, 30, H_CRYSTAL_BAFFLE]);
            }
            rods();
        }
    }
}

module switchHolder()
{
    difference() {
        Z = M_CRYSTAL + Z_CRYSTAL;
        DZ = SWITCH_FRONT - Z;

        translate([0, 0, Z]) union() {
            tube(h=DZ, di=(D_INNER - DD_HOLDER_TUBES), do=D_INNER);
            intersection() {
                cylinder(h=DZ, d=D_INNER);
                translate([-20, D_INNER/2 - 6, 0])
                    cube(size=[40, 10, DZ]);
            }        
        }
        translate([0, 0, M_SWITCH_CENTER]) switch(true);
    }
}

module emitter() {
    N0 = 20;
    N1 = 5;

    ANGLE0 = 360/N0;
    ANGLE1 = 360/N1;

    R = D_INNER / 2;
    T1 = 4;
    DZ = M_EMITTER_BACK - SWITCH_FRONT;

    difference() {
        union() {
            intersection() {
                cylinder(h=200, d=D_INNER);
                translate([0, 0, SWITCH_FRONT]) union() {
                    for(r=[0:(N0-1)])
                        rotate([0, 0, r*ANGLE0]) translate([-1, 0, 0])
                           polygonYZ(2, [[2, 0], [5,4], [6,2], [12, 4], [D_INNER/2,0]]);
                    for(r=[0:(N1-1)]) 
                        rotate([0, 0, r*ANGLE1]) translate([-T1/2, 0, 0])
                            polygonYZ(T1, [
                                [10,0], 
                                [R-3, DZ], 
                                [R, DZ], 
                                [R, 0]]);
                }
            }
            translate([0, 0, SWITCH_FRONT-1.5]) {
                tube(h=2, do=D_INNER, di=D_INNER-12);
            }
        }
        switchHolder();
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
*translate([0, 0, M_SPEAKER]) speaker();
*color("yellow") rods();

DRAW_AFT        = true;
DRAW_FRONT      = false;
DRAW_LED_PLATE  = false;
DRAW_COVER      = false;
DRAW_EMITTER    = false;

if (DRAW_AFT) {
    speakerHolder();
    aftElectronics();
}

if (DRAW_FRONT)
{
    powerPort();
    crystalHolder();
    switchHolder();
}

if (DRAW_LED_PLATE) {
    ledPlate();
}

if (DRAW_COVER) centerCover();

if (DRAW_EMITTER) emitter();
