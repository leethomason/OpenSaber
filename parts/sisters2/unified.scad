use <../shapes.scad>
use <../commonUnified.scad>
include <dim.scad>

DRAW_AFT = true;
DRAW_FORE = false;

EPS = 0.01;
EPS2 = 2 * EPS;

//N_BATT_BAFFLES = nBafflesNeeded(DZ_BAFFLE);
M_JOINT = M_MC_BATTERY + zLenOfBaffles(N_BATT_BAFFLES, DZ_BAFFLE);
T_SECTION = 5;
T_JOINT = 4.5;
DZ_JOINT = 8;

M_BOLT_SECTION =  M_JOINT + DZ_DISPLAY_SECTION;
M_POWER_SECTION = M_BOLT_SECTION + DZ_BOLT_SECTION;
M_SWITCH_SECTION = M_POWER_SECTION + DZ_POWER_SECTION;
DZ_SWITCH_SECTION = M_END - M_SWITCH_SECTION;

DZ_TO_PCB = 0.0;

if (DRAW_AFT) {
    translate([0, 0, M_START]) {
        speakerHolder(D_INNER, DZ_SPKR, 3.0, "std28");
    }

    translate([0, 0, M_MC_BATTERY]) {
        baffleMCBattery(D_INNER, N_BATT_BAFFLES, DZ_BAFFLE, bridgeStyle=2);
        *color("red") battery(D_INNER, "18650");
    }

    translate([0, 0, M_JOINT]) {
        keyJoint(DZ_JOINT, D_INNER, D_INNER - T_JOINT, false, 0);
    }
}

if (DRAW_FORE) {
    *keyJoint(DZ_JOINT, D_INNER, D_INNER - T_JOINT, true, 0);
    difference() {
        union() {
            translate([0, 0, M_JOINT]) {
                difference() {
                    union() {
                        color("plum") pcbHolder(D_INNER, 
                            T_SECTION, 
                            DZ_DISPLAY_SECTION,
                            DZ_TO_PCB,
                            OLED_DY,
                            [OLED_DISPLAY_W, 20, OLED_DISPLAY_L],
                            [
                                [-OLED_DISPLAY_MOUNT_W/2, (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L)/2, "buttress" ],
                                [ OLED_DISPLAY_MOUNT_W/2, (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L)/2, "buttress" ],
                                [-OLED_DISPLAY_MOUNT_W/2, OLED_DISPLAY_MOUNT_L + (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L)/2, "buttress" ],
                                [ OLED_DISPLAY_MOUNT_W/2, OLED_DISPLAY_MOUNT_L + (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L)/2, "buttress" ]
                            ],
                            xShift = OLED_DX,
                            buffButtress=2.0
                        );
                        intersection() {
                            T0 = 6;
                            T1 = 4;
                            cylinder(h=DZ_DISPLAY_SECTION, d=D_INNER);
                            union() {
                                // Buff up the buttresses and the key slot
                                translate([D_INNER/2 - T0, -50, 0]) cube(size=[T0, 100, DZ_DISPLAY_SECTION]);
                                mirror([-1, 0, 0]) translate([D_INNER/2 - T1, -50, 0]) cube(size=[T1, 100, DZ_DISPLAY_SECTION]);
                            }                    
                        }
                    }
                    keyJoint(DZ_JOINT, D_INNER, D_INNER - T_JOINT, true, 0);
                }
            }

            translate([0, 0, M_POWER_SECTION]) {
                color("olive") powerPortRing(D_INNER, T_SECTION, DZ_POWER_SECTION, M_POWER - M_POWER_SECTION);
            }

            translate([0, 0, M_BOLT_SECTION]) {
                color("cadetBlue") boltRing(D_INNER, T_SECTION, DZ_BOLT_SECTION, M_BOLT - M_BOLT_SECTION);
            }

            translate([0, 0, M_SWITCH_SECTION]) {
                switchRing(D_INNER, T_SECTION, DZ_SWITCH_SECTION, M_SWITCH - M_SWITCH_SECTION);
            }
        }
        for(i=[0:1])
            translate([0, 0, M_JOINT + 16 + i*8]) capsule(70, 110, 2, true);
        for(i=[-1:2])
            translate([0, 0, M_JOINT + 16 + i*8]) capsule(160, 200, 2, true);
        for(i=[0:1])
            translate([0, 0, M_POWER + i*8]) capsule(70, 110, 2, true);
        
        // Flat bottom
        translate([-50, -D_INNER/2 - EPS, 0]) cube(size=[100, 1, 500]);
        // Flat top
        translate([-50, D_INNER/2 - 0.4, 0]) cube(size=[100, 100, 500]);
    }
}

echo("Switch-Port capsule 12mm", M_POWER - 6.0, M_SWITCH + 6.0);
echo("Display capsule 8mm", M_JOINT + DZ_TO_PCB + OLED_SCREEN_DZ, M_JOINT + DZ_TO_PCB + OLED_SCREEN_DZ + OLED_SCREEN_Z);

*translate([0, 0, M_END]) color("blue") cylinder(h=1, d=D_INNER);

module cap(z0, z1, d)
{
    hull() {
        translate([0, D_INNER/2, z0]) rotate([-90, 0, 0]) cylinder(h=1, d=d);
        translate([0, D_INNER/2, z1]) rotate([-90, 0, 0]) cylinder(h=1, d=d);
    }
}

*#color("gold") {
    cap(M_SWITCH, M_POWER, 12.0);
    cap(M_JOINT + DZ_TO_PCB + OLED_SCREEN_DZ + 4.0, 
        M_JOINT + DZ_TO_PCB + OLED_SCREEN_DZ + OLED_SCREEN_Z - 4.0, 
        8.0);
    cap(M_BOLT, M_BOLT, 8.0);
}