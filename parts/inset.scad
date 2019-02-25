include <shapes.scad>
include <commonUnified.scad>

// Retaining bolt.
BOLT_D = 4.5;
NUT_W = 8.6;
NUT_Y = 3.4;

SWITCH_BRIDGE_DZ = 5.8;
MID_BRIDGE_DZ = 10;

WOOD_DY = -8;
POST_DY = 5;
POWER_DY = 7.5;
SWITCH_DY = 9.5;

module insetCapsule(extends, dy, deltaD=0)
{
    hull() {
        translate([0, D_OUTER/2 + dy , Z0]) rotate([-90, 0, 0]) cylinder(h=extends, d=D_CAPSULE + deltaD);
        translate([0, D_OUTER/2 + dy, Z1]) rotate([-90, 0, 0]) cylinder(h=extends, d=D_CAPSULE + deltaD);
    }
}

module attachPost()
{
    INSET = NUT_W * 0.6;

    difference() 
    {
        simpleBridge(D_INNER, R_INNER - POST_DY, 5, MID_BRIDGE_DZ);

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


module insetHolder(diameter, outerDiameter, dzSection, dzCenter, dzCenterPort, dzCenterSwitch, diameterCapsule)
{
    Z_MID = dzCenter;
    DZ_PORT = dzCenterPort;
    DZ_SWITCH = dzCenterSwitch;
    D_INNER = diameter;
    R_INNER = D_INNER / 2.0;

    difference() {
        union() {
            translate([0, 0, Z_MID])
                attachPost();

            // Switch holder. Fixme: needs side holders?
            translate([0, 0, Z_MID + DZ_SWITCH])
                simpleBridge(D_INNER, R_INNER - SWITCH_DY, 4, SWITCH_BRIDGE_DZ);

            // Power holder.
            translate([0, 0, Z_MID + DZ_PORT]) {
                simpleBridge(D_INNER, R_INNER - POWER_DY, 3, 12, 4);
            }
        }
        wood();
        // Underside of power port
        translate([0, 0, Z_MID + DZ_PORT]) {
            rotate([-90, 0, 0]) {
                // Initially measured.
                cylinder(h=50, d=8.0);
                cylinder(h=R_INNER - POWER_DY - 1.5, d=11.5);
            }
        }
    }

    difference() {
        DY = 12;
        BAFSIZE = 4;
        NBAF = 8;

        union() {
            // Baffles.
            for(i=[0:NBAF-1]) {
                translate([0, 0, Z_MID - (NBAF-0.5) * BAFSIZE + i*BAFSIZE*2]) {
                    difference() {
                        oneBaffle(D_INNER, BAFSIZE, bridge=(i<(NBAF-1)), battery=false, mc=false, cutout=false);
                        hull() {
                            cylinder(h=BAFSIZE*1.1, d=D_INNER * 0.65);
                            translate([0, -2.5, 0])
                                cylinder(h=BAFSIZE*1.1, d=D_INNER * 0.7);
                        }
                    }
                }
            }
            intersection() {
                capsule(10, WOOD_DY-4, 4);
                cylinder(h=100, d=D_INNER);
            }
        }
        wood();
        capsule(10, WOOD_DY-8, -4);

        translate([0, 0, Z_MID + DZ_PORT]) {
            // Access hole
            rotate([90, 0, 0]) {
                cylinder(h=50, d=15.0);
            }
        }
    }
}

