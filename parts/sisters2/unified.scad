use <../shapes.scad>
use <../commonUnified.scad>
include <dim.scad>

//N_BATT_BAFFLES = nBafflesNeeded(DZ_BAFFLE);
M_JOINT = M_MC_BATTERY + zLenOfBaffles(N_BATT_BAFFLES, DZ_BAFFLE);
T_SECTION = 5;
T_JOINT = 4;
DZ_JOINT = 8;

M_POWER_SECTION = M_JOINT + DZ_DISPLAY_SECTION;
M_SWITCH_SECTION = M_POWER_SECTION + DZ_POWER_SECTION;
DZ_SWITCH_SECTION = M_END - M_SWITCH_SECTION;

translate([0, 0, M_START]) {
    speakerHolder(D_INNER, DZ_SPKR, 3.0, "std28");
}

*translate([0, 0, M_MC_BATTERY]) {
    baffleMCBattery(D_INNER, N_BATT_BAFFLES, DZ_BAFFLE);
    color("red") battery(D_INNER, "18650");
}

*translate([0, 0, M_JOINT]) {
    keyJoint(DZ_JOINT, D_INNER, D_INNER - T_JOINT, false, 0);
}

translate([0, 0, M_JOINT]) {
    difference() {
        pcbHolder(D_INNER, 
            T_SECTION, 
            DZ_DISPLAY_SECTION,
            2,  // dzToPCB
            8,  // dyPCB
            [OLED_DISPLAY_W, 20, OLED_DISPLAY_L],
            [
                [-OLED_DISPLAY_MOUNT_W/2, (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L)/2, "buttress" ],
                [ OLED_DISPLAY_MOUNT_W/2, (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L)/2, "buttress" ],
                [-OLED_DISPLAY_MOUNT_W/2, OLED_DISPLAY_MOUNT_L + (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L)/2, "buttress" ],
                [ OLED_DISPLAY_MOUNT_W/2, OLED_DISPLAY_MOUNT_L + (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L)/2, "buttress" ]
            ]
        );
        keyJoint(DZ_JOINT, D_INNER, D_INNER - T_JOINT, true, 0);
    }
}

translate([0, 0, M_POWER_SECTION]) {
    echo(M_POWER_SECTION);
    powerPortRing(D_INNER, T_SECTION, DZ_POWER_SECTION, M_POWER - M_POWER_SECTION);
}

translate([0, 0, M_SWITCH_SECTION]) {
    echo(M_SWITCH_SECTION);
    switchRing(D_INNER, T_SECTION, DZ_SWITCH_SECTION, M_SWITCH - M_SWITCH_SECTION);
}

translate([0, 0, M_END]) color("blue") cylinder(h=1, d=D_INNER);