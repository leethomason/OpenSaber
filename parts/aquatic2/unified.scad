include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>

$fn = 80;

T = 4;
FRONT_T = 4;
JUNCTION = 8;
EPS = 0.01;
EPS2 = 2 * EPS;

DRAW_AFT     = false;
DRAW_CRYSTAL = false;
DRAW_FRONT   = true;
DRAW_GRIP_ALIGNMENT = false;

N_BATT_BAFFLES = nBafflesNeeded(H_BUTTRESS);
DZ_SPKR = 3;
EXTRA_BAFFLE = 4;
M_BAFFLE_FRONT = M_POMMEL_FRONT + zLenOfBaffles(N_BATT_BAFFLES, H_BUTTRESS) + EXTRA_BAFFLE;

module flatBottom() {
    translate([-50, -D_AFT/2 - EPS, M_WAY_BACK]) {
        cube(size=[100, 0.5, H_FAR]);
    }
}

module dotstars() {
    rotate([0, 0, 180]) translate([0, 8, M_DOTSTARS]) {
        dotstarLED(N_DOTSTARS, 20);
        dotstarStrip(N_DOTSTARS+1, 4, 5.2);
    }
}

if (DRAW_GRIP_ALIGNMENT) {
    color("red") {
        translate([-D_SABER_OUTER/2, -1, M_GRIP_BACK]) {
            cube(size=[D_SABER_OUTER, 2, M_FRONT - M_GRIP_BACK]);
        }
        echo("Crystal space=", M_GRIP_BACK - M_BAFFLE_FRONT - H_BUTTRESS);
    }
}

if (DRAW_AFT) {
    translate([0, 0, M_POMMEL_BACK]) {
        // Speaker holder locking ring.
        speakerHolder(D_AFT, M_POMMEL_FRONT - M_POMMEL_BACK, DZ_SPKR, "cls28");
    }
    translate([0, 0, M_POMMEL_FRONT]) {
        baffleMCBattery(D_AFT, N_BATT_BAFFLES, H_BUTTRESS, dFirst=D_AFT_RING, dzFirst=DZ_AFT_RING, extraBaffle=EXTRA_BAFFLE);
    }
    translate([0, 0, M_BAFFLE_FRONT]) {
        keyJoint(JUNCTION, D_AFT, D_AFT - T, 0.5, tJoint=true);
    }

    *color("yellow") translate([0, 0, M_POMMEL_FRONT])
        battery(D_AFT);
}

X_CRYSTAL           =  13;
Y_CRYSTAL           =   8.5;
Z_CRYSTAL           =  39.0;
Y_CRYSTAL_TR = R_AFT - 2 - Y_CRYSTAL/2;
N_CRYSTAL_BAFFLES = 4;


module rods() {
    RX = 8;
    RY = 4;
    translate([RX, RY, M_BAFFLE_FRONT]) {
        cylinder(h=(N_CRYSTAL_BAFFLES+1) * H_BUTTRESS * 2, d=3.4);
    }
    mirror([-1, 0, 0]) translate([RX, RY, M_BAFFLE_FRONT]) {
        cylinder(h=(N_CRYSTAL_BAFFLES+1) * H_BUTTRESS * 2, d=3.4);
    }
}


if (DRAW_CRYSTAL) {
    difference() { // top level, holes & bottom cut from here.
        union() {
            *translate([0, Y_CRYSTAL_TR, M_BAFFLE_FRONT + H_BUTTRESS]) {
                crystal(X_CRYSTAL, Y_CRYSTAL, Z_CRYSTAL);
            }

            difference() {
                // "wall" for back of crystal and dotstar
                intersection() {
                    cylinder(h=H_FAR, d=D_AFT);
                    translate([-50, 5, M_BAFFLE_FRONT + H_BUTTRESS/2]) {
                        cube(size=[100, 100, H_BUTTRESS/2]);
                    }
                }

                // Punch out the dotstar holder.
                DOT = 5.8;
                translate([-DOT/2, Y_CRYSTAL_TR-DOT/2, M_BAFFLE_FRONT]) {
                    cube(size=[DOT, DOT, 10]);
                }
            }

            difference()
            {
                union() {
                    translate([0, 0, M_BAFFLE_FRONT]) {
                        oneBaffle(D_AFT, H_BUTTRESS, battery=false, cutout=false);
                    }
                    for(i=[1:N_CRYSTAL_BAFFLES]) {
                        translate([0, 0, M_BAFFLE_FRONT + H_BUTTRESS*2*i]) {
                            oneBaffle(D_AFT, H_BUTTRESS, battery=false, cutout=true, cutoutHigh=false);
                        }
                    }
                    // Side parts for key-joint to connect.
                    intersection() {
                        union() {
                            translate([D_AFT/2 - 4, -50, M_BAFFLE_FRONT]) cube(size=[100, 100, H_BUTTRESS*3]);
                            mirror([-1, 0, 0]) translate([D_AFT/2 - 4, -50, M_BAFFLE_FRONT]) cube(size=[100, 100, H_BUTTRESS*3]);
                        }
                        cylinder(h=H_FAR, d=D_AFT);
                    }
                }
                // space for dotstar:
                translate([-8, 0, M_BAFFLE_FRONT-EPS]) {
                    cube(size=[16, 100, H_BUTTRESS/2+EPS]);
                }

                translate([0, Y_CRYSTAL_TR, M_BAFFLE_FRONT + H_BUTTRESS]) {
                    crystal(X_CRYSTAL, Y_CRYSTAL, Z_CRYSTAL);
                }
                W = 8;
                translate([-W/2, Y_CRYSTAL_TR, M_BAFFLE_FRONT + H_BUTTRESS]) {
                    cube(size=[W, 100, Z_CRYSTAL]);
                }
                translate([0, 0, M_BAFFLE_FRONT]) {
                    keyJoint(JUNCTION, D_AFT, D_AFT - T, 0, tJoint=true);
                }
            }

            // FIXME: adjust origin to give crystal space, if needed.

            translate([0, 0, M_GRIP_BACK + 2]) {
                oneBaffle(D_AFT, H_BUTTRESS,
                        battery=true,       // for aesthetics
                        cutout=false,
                        cutoutHigh=true,
                        bridge=false);
            }
            translate([0, 0, M_GRIP_BACK + 2 + H_BUTTRESS]) {
                columnJoint(JUNCTION-2, D_AFT, 0.1);
            }
        }
        // Front part global removal:
        flatBottom();
        rods();
        dotstars();
    }
}

if (DRAW_FRONT) {
    difference() { // top level, holes & bottom cut from here.
        union() {
            M_START = M_GRIP_BACK + 2 + H_BUTTRESS;
            DZ_POWER = (M_POWER + 7) - M_START;
            DZ_SWITCH = M_FRONT - (M_START + DZ_POWER);

            translate([0, 0, M_START]) {
                powerPortRing(D_AFT, T, DZ_POWER, M_POWER - M_START);
            }
            translate([0, 0, M_START + DZ_POWER]) {
                switchRing(D_AFT, T, DZ_SWITCH, M_SWITCH - (M_START + DZ_POWER));
            }
        }
        // Front part global removal:
        flatBottom();
        rods();
        translate([0, 0, M_GRIP_BACK + 2 + H_BUTTRESS]) {
            columnJoint(JUNCTION-2, D_AFT, 0);
        }

        // Bottom access.
        /*
        translate([0, 0, M_SWITCH]) 
            rotate([90, 0, 0]) cylinder(h=100, d=12);
        */
        translate([0, 0, M_POWER]) {
            rotate([90, 0, 0]) cylinder(h=100, d=12);
        }

        // Side vents
        for(i=[0:2]) {
            THETA = 29;
            OFFSET = 0;
            BOLT = 3.4;

            if (i == 1) rotate([0, 0, 180 + -THETA + OFFSET + i*THETA]) {
                translate([0, 0, M_GRIP_BACK + 22]) {
                    hull() {
                        rotate([0, 90, 0]) cylinder(h=100, d=BOLT);        
                        translate([0, 0, 16]) {
                            rotate([0, 90, 0]) cylinder(h=100, d=BOLT);
                        }
                    }
                }
            }
            rotate([0, 0, -THETA + OFFSET + i*THETA]) {
                translate([0, 0, M_GRIP_BACK + 22]) {
                    hull() {
                        rotate([0, 90, 0]) cylinder(h=100, d=BOLT);        
                        translate([0, 0, 16]) {
                            rotate([0, 90, 0]) cylinder(h=100, d=BOLT);
                        }
                    }
                }
            }
        }
        // Flat top. Still debating this one.
        translate([-50, D_AFT/2 - 1, M_GRIP_BACK])
            cube(size=[100, 10, 100]);
        dotstars();
    }
    // Bolt mark
    *color("red") translate([0, 0, M_GRIP_BACK + 58.5 / 2]) rotate([0, -90, 0]) cylinder(h=20, d=3.2);
}
