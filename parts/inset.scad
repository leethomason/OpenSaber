include <shapes.scad>
include <commonUnified.scad>

$fn = 80;

// Retaining bolt.
BOLT_D = 4.5;
NUT_W = 8.6;
NUT_Y = 3.4;

SWITCH_BRIDGE_DZ = 5.8;
MID_BRIDGE_DZ = 10;

WOOD_DY = -8;
POST_DY = 5;
POWER_DY = 7.5;
SWITCH_DY = 10.5;

module insetCapsule(extends, dy, deltaD=0)
{
    hull() {
        translate([0, D_OUTER/2 + dy , Z0]) rotate([-90, 0, 0]) cylinder(h=extends, d=D_CAPSULE + deltaD);
        translate([0, D_OUTER/2 + dy, Z1]) rotate([-90, 0, 0]) cylinder(h=extends, d=D_CAPSULE + deltaD);
    }
}

module attachPost(diameter)
{
    INSET = NUT_W * 0.6;

    difference() 
    {
        //simpleBridge(diameter, diameter/2 - POST_DY, 5, MID_BRIDGE_DZ);
        BRIDGE_T = 8;
        intersection() {
            translate([0, 0, -MID_BRIDGE_DZ/2 - 0.5])
                cylinder(h=MID_BRIDGE_DZ + 1, d=diameter - 1);
            translate([-20, diameter/2 - POST_DY - BRIDGE_T, -MID_BRIDGE_DZ/2])
                cube(size=[40, BRIDGE_T, MID_BRIDGE_DZ]);
        }


        // Bolt hole
        rotate([-90, 0, 0])
            cylinder(h=20, d=BOLT_D);

        // Nut retainer
        translate([-NUT_W/2, 5, -20])
            cube(size=[NUT_W, NUT_Y, 40]);

        translate([-INSET/2, 0, -20])
            cube(size=[INSET, 6, 40]);

        // slot to lift out inset.
        HOOK_X = 8;
        HOOK_Z = 2;
        translate([-HOOK_X/2, 5, -HOOK_Z/2])
            cube(size=[HOOK_X, 10, HOOK_Z]);
    }
}

module insetBaffle(diameter, dzBaffle, bridge)
{
    difference() {
        oneBaffle(diameter, dzBaffle, bridge=bridge, battery=false, mc=false, conduit=true);
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
                    dzSection, 
                    dzCenter, 
                    dzCenterPort, 
                    dzCenterSwitch, 
                    diameterCapsule,
                    dzBaffle,
                    bridgeStyle=1,
                    bridgeStyleArray=undef,
                    pinHeaderHolder=false)
{
    Z_MID = dzCenter;
    DZ_PORT = dzCenterPort;
    DZ_SWITCH = dzCenterSwitch;
    D_INNER = diameter;
    R_INNER = D_INNER / 2.0;
    Y_INSET = R_INNER - 5;
    rCapsule = diameterCapsule / 2;

    //color("red") cylinder(d=10, h=dzSection);

    // Bridges
    X_SWITCH = 6.5;
    Y_SWITCH = 3.0;
                    
    difference() {
        union() {
            translate([0, 0, Z_MID])
                attachPost(D_INNER);

            // Switch flat version + header mount
            intersection() {
                BRIDGE_T = 3;
                cylinder(h=300, d=D_INNER);                
                translate([0, 0, Z_MID + DZ_SWITCH - SWITCH_BRIDGE_DZ/2]) {
                    translate([-20, R_INNER - SWITCH_DY - BRIDGE_T]) {
                        cube(size=[40, BRIDGE_T, SWITCH_BRIDGE_DZ]);
                    }

                    translate([X_SWITCH/2, R_INNER - SWITCH_DY, 0]) {
                        cube(size=[50, Y_SWITCH, SWITCH_BRIDGE_DZ]);
                    }

                    mirror([-1, 0, 0]) translate([X_SWITCH/2, R_INNER - SWITCH_DY, 0]) {
                        cube(size=[50, Y_SWITCH, SWITCH_BRIDGE_DZ]);
                    }

                    if (pinHeaderHolder)
                        headerHolder(diameter, R_INNER - SWITCH_DY - 9.9);
                }
            }

            // Power holder.
            translate([0, 0, Z_MID + DZ_PORT]) {
                BRIDGE_T = 3;
                intersection() {
                    translate([0, 0, -7 - 0.5])
                        cylinder(h=14 + 1, d=D_INNER - 1);
                    translate([-20, R_INNER - POWER_DY - BRIDGE_T, -7])
                        cube(size=[40, BRIDGE_T, 14]);
                }
            }
        }

        // Power port
        translate([0, 0, Z_MID + DZ_PORT]) {
            rotate([-90, 0, 0]) {
                cylinder(h=50, d=8.0);
                cylinder(h=R_INNER - POWER_DY - 1.5, d=11.5);
            }
        }
    }

    // The inset holder.
    intersection()
    {
        translate([0, 0, -50]) cylinder(h=200, d=D_INNER);
        translate([0, Y_INSET, Z_MID]) {
            difference() {
                DY = -3;
                translate([0, DY, 0])
                    zCapsule(DZ_PORT, DZ_SWITCH, rCapsule+2);
                zCapsule(DZ_PORT, DZ_SWITCH, rCapsule);          // the actual wood
                translate([0, DY - EPS*2, 0])
                    zCapsule(DZ_PORT, DZ_SWITCH, rCapsule-2);

                stockX = diameterCapsule;
                stockY = outerDiameter / 2 - (Y_INSET);
                stockZ = abs(DZ_PORT - DZ_SWITCH) + diameterCapsule;
                echo("Stock size:", stockX, stockY, stockZ);
            }
        }
    }

    nBaffle = floor((dzSection+dzBaffle) / (dzBaffle*2)); 

    difference() {
        union() {
            for(i=[0:nBaffle-1]) {
                translate([0, 0, i*dzBaffle*2]) {
                    style = (bridgeStyleArray && i < len(bridgeStyleArray)) ? bridgeStyleArray[i] : bridgeStyle;
                    insetBaffle(diameter, dzBaffle, style);
                }
            }
        }
        // Removes the front bridge hanging out.
        translate([0, 0, dzSection]) cylinder(h=dzBaffle*2, d=diameter);

        // Removes the space for the inset.
        translate([0, Y_INSET, Z_MID])
            difference()
                translate([0, -4, 0])
                    zCapsule(DZ_PORT, DZ_SWITCH, rCapsule+2);

        // Access to the power port.
        translate([0, 0, Z_MID + DZ_PORT])
            rotate([90, 0, 0])
                cylinder(h=50, d=15.0);
    }
    // Cap the end.
    translate([0, 0, dzSection - dzBaffle]) {
        insetBaffle(diameter, dzBaffle, bridgeStyle);
    }
}

insetHolder(32.2, 37.9, 
    52, 24,     // section, center
    -13.0, 13.0, 16.0, 4.0);
