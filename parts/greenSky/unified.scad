include <dim.scad>
include <../commonUnified.scad>
use <../shapes.scad>

$fn = 90;

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
X_BRIDGE = X_MC/2+1;
D_BAFFLE_INNER = D_INNER-6;

module cBaffle(threads=false) {
    intersection() {
        cylinder(d=D_INNER, h=H_CRYSTAL_BAFFLE*2);

        difference() {
            union() {        
                translate([0, DY_CRYSTAL, 0]) {
                    cylinder(h=H_CRYSTAL_BAFFLE, d=D_CRYSTAL_BAFFLE);
                }
                tube(h=H_CRYSTAL_BAFFLE, do=D_INNER, di=D_BAFFLE_INNER);
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


// Move M_END_BAFFLE to dim file? This is actually pretty key:
NUM_BAFFLES = 8;
M_END_BAFFLE = M_POMMEL + H_BUTTRESS*2*(1 + NUM_BAFFLES);

DZ_PORT = M_PORT_CENTER + ZPAD_PORT - M_END_BAFFLE;
PORT_FRONT = DZ_PORT + M_END_BAFFLE;

SWITCH_BACK = M_SWITCH_CENTER - 9;
T_SWITCH_RING = 2;

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
    difference() {
        // One extra baffle; the common code counts the ring.
        translate([0, 0, M_POMMEL]) 
            baffleMCBattery(D_INNER, NUM_BAFFLES+1, H_BUTTRESS, D_RING, Z_RING);
        rods();
    }
    key();
}

DD_HOLDER_TUBES = 4;
DZ_LED_PLATE = 3;

module ledPlate()
{
    intersection() {
        cylinder(d=D_INNER - DD_HOLDER_TUBES, h=200);

        difference() {
            color("aqua")  
            union() {        
                translate([0, DY_CRYSTAL, M_CRYSTAL - DZ_LED_PLATE]) {
                    cylinder(h=DZ_LED_PLATE * 0.98, d=D_CRYSTAL_BAFFLE);
                }
            }
            translate([0, DY_CRYSTAL, M_CRYSTAL - DZ_LED_PLATE]) 
                cylinder(h=DZ_LED_PLATE+EPS, d=3.2); // LED
        }        
    }    
}

module powerPort() 
{
    DZ = 3;
    difference() {
        translate([0, 0, M_END_BAFFLE])
            powerPortRing(D_INNER, DD_HOLDER_TUBES, DZ_PORT, M_PORT_CENTER - M_END_BAFFLE);

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
        translate([0, DY_CRYSTAL, M_CRYSTAL]) crystal();
        rods();        
    }
}

module centerCover()
{
    // Cover
    translate([0, 0, PORT_FRONT]) 
    difference() {
        tube(h=M_CRYSTAL + Z_CRYSTAL - PORT_FRONT, inner=(D_INNER-DD_HOLDER_TUBES)/2, outer=D_INNER/2);
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

        rods();
    }
    intersection() {
        union() {
            cylinder(h=200, d=D_BAFFLE_INNER);    
            intersection() {
                translate([-20, 0, 0]) cube(size=[40, 40, 200]);
                cylinder(h=200, d=D_INNER);
            }
        }

        difference() {
            DZ = H_CRYSTAL_BAFFLE*2;
            union() {
                translate([10, -4, PORT_FRONT + DZ]) 
                    cube(size=[10, 30, DZ]);
                mirror([1,0,0]) translate([10, -4, PORT_FRONT + DZ]) 
                    cube(size=[10, 30, DZ]);
                translate([10, -4, PORT_FRONT + H_CRYSTAL_BAFFLE*12]) 
                    cube(size=[10, 30, DZ]);
                mirror([1,0,0]) translate([10, -4, PORT_FRONT + H_CRYSTAL_BAFFLE*12]) 
                    cube(size=[10, 30, DZ]);
            }
            rods();
        }
    }
}

module switchHolder()
{
    Z = M_SWITCH_CENTER - ZPAD_SWITCH;
    DZ = M_EMITTER_BACK - Z;

    
    difference() {
        translate([0, 0, Z])
            switchRing(D_INNER, DD_HOLDER_TUBES, DZ, M_SWITCH_CENTER - Z);

        translate([0, 0, Z+4])  capsule(60, 70);
        translate([0, 0, Z+10])  capsule(60, 70);

        translate([0, 0, Z+4])  capsule(-60, -70);
        translate([0, 0, Z+10])  capsule(-60, -70);

        rods();
    }
    intersection() {
        translate([0, 0, Z]) 
            cylinder(h=DZ, d=D_INNER);
        union() {
            translate([0, -20, M_EMITTER_BACK - 8])
                cubePair(10, size=[20, 24, 40]);
            difference() {
                translate([0, -20, M_EMITTER_BACK - 11])
                    cubePair(10, size=[20, 19, 4]);
                rods();
            }
        }
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
*translate([0, 0, M_SPEAKER]) speakerBass22();
*color("yellow") rods();

DRAW_AFT        = false;
DRAW_FRONT      = true;
DRAW_LED_PLATE  = false;
DRAW_COVER      = false;

if (DRAW_AFT) {
    difference() {
        translate([0, 0, M_BACK]) 
            speakerHolder(D_POMMEL, M_POMMEL - M_BACK, M_SPEAKER - M_BACK);
        rods();
    }
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

if (DRAW_COVER) 
    centerCover();
