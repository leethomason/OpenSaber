include <shapes.scad>
include <commonUnified.scad>

$fn = 80;

SWITCH_BRIDGE_DZ = 5.8;
INNER_VIEW = false;

WOOD_DY = -8;
POWER_DY = 2.5;
EPS = 0.01;
ESP2 = EPS*2;

module attachPost(diameter, postDY, capsuleWidth)
{
    BRIDGE_DY = 5.5 + 3.4;
    BRIDGE_DZ = 10;

    BOLT_D = 4.5;
    NUT_W = 8.6;
    NUT_Y = 3.4;
    
    HOOK_X = 20;
    HOOK_Z = 2;

    INSET_DZ = 2;

    Y_TOP = diameter / 2 - postDY;

    intersection() {
        translate([0, 0, -50]) cylinder(h=100, d=diameter);
        union() {
            // bridge
            difference() {
                translate([-50, Y_TOP - BRIDGE_DY, -BRIDGE_DZ/2]) {
                    cube(size=[100, BRIDGE_DY, BRIDGE_DZ]);
                }
                // space for nut
                translate([-NUT_W/2, Y_TOP - BRIDGE_DY - EPS, -BRIDGE_DZ/2 - EPS]) {
                    cube(size=[NUT_W, NUT_Y + EPS, BRIDGE_DZ + EPS2]);
                }
                // bolt
                rotate([-90, 0, 0])
                    cylinder(h=20, d=BOLT_D);

                translate([-capsuleWidth/2, 0, -INSET_DZ/2])
                    cube(size=[capsuleWidth, 100, INSET_DZ]);
            }

            Y_HOLDER = Y_TOP - BRIDGE_DY;
            MULT = 0.3;
            translate([0, 0, -INSET_DZ/2]) {
                polygonXY(h=INSET_DZ, points=[
                    [-50,  Y_HOLDER],
                    [-NUT_W * MULT, Y_HOLDER],
                    [-50, -50]
                ]);
                mirror([-1,0,0]) polygonXY(h=INSET_DZ, points=[
                    [-50,  Y_HOLDER],
                    [-NUT_W * MULT, Y_HOLDER],
                    [-50, -50]
                ]);
            }
        }
    }
}

module insetBaffle(diameter, dzBaffle, bridge, noBottom=false)
{
    difference() {
        oneBaffle(diameter, dzBaffle, 
            bridge=bridge, 
            battery=false, 
            mc=true, 
            conduit=true, 
            noBottom=noBottom);

        hull() {
            cylinder(h=dzBaffle*1.1, d=diameter * 0.65);
            translate([0, -2.5, 0])
                cylinder(h=dzBaffle*1.1, d=diameter * 0.7);
        }
    }
}


// Origin top, center, back of pins.
module headerHolder(diameter, dy)
{
    HEADER_HOLDER_T = 2.0;
    HEADER_HOLDER_DZ = 2.6;
    DX = 10;
    D_OPEN = 6.3;

    //intersection()
    {
        //translate([0, 0, -20]) cylinder(h=300, d=diameter);
        translate([0, dy, 0])
            difference() {
                translate([-(DX/2 + HEADER_HOLDER_T), -20, -(HEADER_HOLDER_DZ + HEADER_HOLDER_T)]) {
                    cube(size=[DX + 2*HEADER_HOLDER_T, 20, HEADER_HOLDER_DZ + 2*HEADER_HOLDER_T]);
                }
                translate([-DX/2, -20, -HEADER_HOLDER_DZ])
                    cube(size=[DX, 21, HEADER_HOLDER_DZ]);
                translate([-D_OPEN/2, -20, -HEADER_HOLDER_DZ-HEADER_HOLDER_T-1])
                    cube(size=[D_OPEN, 21, HEADER_HOLDER_DZ+HEADER_HOLDER_T+1]);
        }
    }
}


/*
    Creates a section with an inset holder, intended to give access to the switch
    and power, as well as lock the other case to the inner carriage.
*/
module insetHolder( diameter,
                    outerDiameter,
                    diameterCapsule,
                    dzSection,
                    dzBaffle,
                    dzStart,
                    dzEnd,
                    dzBolt = 0,
                    dzPort = 0,
                    dzSwitch = 0,
                    dzUSB = 0,
                    bridgeStyle=1,
                    bridgeStyleArray=undef,
                    pinHeaderHolder=false,
                    firstButtressFullRing=true,
                    roundRect=0,
                    dyInset=5,
                    dySwitch=5.5)
{
    rCapsule = diameterCapsule / 2;
    rInner = diameter / 2;
    yInset = rInner - dyInset;
    dzMid = (dzStart + dzEnd) / 2;
    dzCap = dzEnd - dzStart - diameterCapsule;
    dzA = dzStart + rCapsule;
    dzB = dzEnd - rCapsule;

    // check alignment
    // color("red") cylinder(d=10, h=dzSection);

    // Bridges
    X_SWITCH = 6.5;
    Y_SWITCH = 3.0;

    X_USB_OUTER = 25.0;     // Size of the outer holder. Bigger than USB PCB.
    X_USB_INNER = 13.0;     // Space inside for wires and plug
    Z_USB_OUTER = 6;        // Outer holder size
    USB_SLOT = 1.8;         // PCB cutout space
    X_USB_SLOT = 21.0;      // Width of PCB
    DEPTH_USB_SLOT = 11.0;   // Height of PCB
                    
    difference() {
        union() {
            if (dzBolt)
                translate([0, 0, dzBolt])
                    attachPost(diameter, dyInset, rCapsule*2-4);

            // Switch flat version + header mount
            if (dzSwitch) {
                intersection() {
                    BRIDGE_T = 3;
                    cylinder(h=300, d=diameter);
                    translate([0, 0, dzSwitch - SWITCH_BRIDGE_DZ/2]) {
                        translate([-20, rInner - dySwitch - dyInset - BRIDGE_T]) {
                            cube(size=[40, BRIDGE_T, SWITCH_BRIDGE_DZ]);
                        }

                        translate([X_SWITCH/2, rInner - dySwitch - dyInset, 0]) {
                            cube(size=[50, Y_SWITCH, SWITCH_BRIDGE_DZ]);
                        }

                        mirror([-1, 0, 0]) translate([X_SWITCH/2, rInner - dySwitch - dyInset, 0]) {
                            cube(size=[50, Y_SWITCH, SWITCH_BRIDGE_DZ]);
                        }

                        if (pinHeaderHolder)
                            headerHolder(diameter, rInner - SWITCH_DY - dyInset - 9.9);
                    }
                }
            }

            // Power holder.
            if (dzPort) {
                translate([0, 0, dzPort]) {
                    BRIDGE_T = 3;
                    intersection() {
                        translate([0, 0, -7 - 0.5])
                            cylinder(h=14 + 1, d=diameter - 1);
                        translate([-20, rInner - POWER_DY - dyInset - BRIDGE_T, -7])
                            cube(size=[40, BRIDGE_T, 14]);
                    }
                }
            }

            // USB holder

            if(dzUSB) {
                intersection() {
                    cylinder(h=300, d=diameter);
                    difference() {
                        translate([-X_USB_OUTER/2, yInset - 100, dzUSB - Z_USB_OUTER/2])
                            cube(size=[X_USB_OUTER, 100-EPS, Z_USB_OUTER]);
                        translate([-X_USB_INNER/2, yInset - 100, dzUSB - Z_USB_OUTER/2 - EPS])
                            cube(size=[X_USB_INNER, 100, Z_USB_OUTER + EPS*2]);
                    }
                }
            }
        }

        // Power port
        if (dzPort) {
            translate([0, 0, dzPort]) {
                rotate([-90, 0, 0]) {
                    cylinder(h=50, d=8.0);
                    cylinder(h=rInner - POWER_DY - dyInset - 1.5, d=11.5);
                }
            }
        }

        if (dzUSB) {
            translate([-X_USB_SLOT/2, yInset - DEPTH_USB_SLOT, dzUSB - USB_SLOT/2])
                cube(size=[X_USB_SLOT, 100, USB_SLOT]);
        }

    }

    // The inset holder.
    difference() {
        intersection()
        {
            translate([0, 0, -50]) cylinder(h=200, d=diameter);
            translate([0, yInset, 0]) {
                difference() {
                    DY = -3;
                    translate([0, DY, 0])
                        zCapsule(dzA, dzB, rCapsule+2, roundRect);
                    zCapsule(dzA, dzB, rCapsule, roundRect);          // the actual wood
                    translate([0, DY - EPS*2, 0])
                        zCapsule(dzA, dzB, rCapsule-2, roundRect);

                    stockX = diameterCapsule;
                    stockY = outerDiameter / 2 - yInset;
                    stockZ = abs(dzA - dzB) + diameterCapsule;
                    echo("Stock size:", stockX, stockY, stockZ);
                }
            }
        }
        if (dzUSB) {
            translate([-X_USB_SLOT/2, 0, dzUSB - USB_SLOT/2])
                cube(size=[X_USB_SLOT, 100, USB_SLOT]);
        }
    }

    nBaffle = floor((dzSection+dzBaffle) / (dzBaffle*2));

    difference() {
        union() {
            for(i=[0:nBaffle-1]) {
                translate([0, 0, i*dzBaffle*2]) {
                    style = (bridgeStyleArray && i < len(bridgeStyleArray)) ? bridgeStyleArray[i] : bridgeStyle;
                    if (!INNER_VIEW)
                        insetBaffle(diameter, dzBaffle, style, noBottom=!firstButtressFullRing || i>0);
                }
            }
        }
        // Removes the front bridge hanging out.
        translate([0, 0, dzSection]) cylinder(h=dzBaffle*2, d=diameter);

        // Removes the space for the inset.
        translate([0, yInset - 4, 0])
            zCapsule(dzA, dzB, rCapsule+2, roundRect);

        // Access to the power port.
        if (dzPort)
            translate([0, 0, dzPort])
                rotate([90, 0, 0])
                    cylinder(h=50, d=13.0);
        if (dzSwitch)
            translate([0, 0, dzSwitch])
                rotate([90, 0, 0])
                    cylinder(h=50, d=13.0);
    }
    // Cap the end.
    translate([0, 0, dzSection - dzBaffle]) {
        insetBaffle(diameter, dzBaffle, bridgeStyle);
    }
}

/*
module insetHolder( diameter,
                    outerDiameter,
                    diameterCapsule,
                    dzSection,
                    dzBaffle,
                    dzStart,
                    dzEnd,
                    dzBolt = 0,
                    dzPort = 0,
                    dzSwitch = 0,
                    dzUSB = 0
                    bridgeStyle=1,
                    bridgeStyleArray=undef,
                    pinHeaderHolder=false,
                    roundRect=0)
*/

insetHolder(
    32.2,
    37.9,
    16.0,
    80,
    4,
    10,
    70,
    40, 20, 60,
    //dzUSB=60,
    roundRect=3.175/2);
