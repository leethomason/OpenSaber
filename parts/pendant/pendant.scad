$fn = 90;

DRAW_BOTTOM =  true;
DRAW_TOP = false;
CUTOUT = true;

CASE_D = 9.5;
BATTERY_D = 7.9 + 0.5;
BATTERY_Z = 3.6;
LOWER_Z = 17.0;          // The lower section. 12mm in the 2nd section.

/*
PASS_0_Z = 22;
PASS_0_DZ = 6;
PASS_0_DX = 6;
PASS_1_Z = 30;
PASS_1_DZ = 7;
PASS_1_DX = 6;
*/
/*
SWITCH_RING_0_Z = 21;
SWITCH_RING_1_Z = 28;
SWITCH_RING_D = 7;
TOP_SPRING_Z = 2.0;
*/ 

SPRING_Z = 2.0;
SPRING_D = 6.0;
FLOOR_Z  = 1.0;
FLOOR_D  = 5.0;
LED_LIFT_Z = 2.0;

if (DRAW_BOTTOM) {
    difference() {
        cylinder(h=LOWER_Z, d=CASE_D);

        // Space for LED and resistor
        cylinder(h=LED_LIFT_Z, d=BATTERY_D);
        // Holds spring in place
        translate([0, 0, LED_LIFT_Z]) 
            cylinder(h=FLOOR_Z, d=FLOOR_D);
        // Spring
        translate([0, 0, LED_LIFT_Z + FLOOR_Z]) 
            cylinder(h=SPRING_Z, d=SPRING_D);
        // Batteries and top spring
        translate([0, 0, LED_LIFT_Z + FLOOR_Z + SPRING_Z]) 
            cylinder(h=LOWER_Z, d=BATTERY_D);

        if (CUTOUT) translate([-20, -20, 0]) cube(size=[40, 20, 100]);
    }
}

color("yellow") translate([0, 0, LED_LIFT_Z + FLOOR_Z + SPRING_Z]) {
    for(i=[0:2]) {
        translate([0, 0, i*BATTERY_Z]) {
            translate([0, 0, 1]) cylinder(h=BATTERY_Z-1, d=BATTERY_D);
            cylinder(h=1, d=BATTERY_D-1);
        }
    }
}

/*
if (DRAW_TOP) {
    translate([0, 0, LOWER_Z]) difference() {
        cylinder(h=CASE_Z - LOWER_Z, d=CASE_D);

        cylinder(h=TOP_SPRING_Z, d=SPRING_D);
        translate([0, 0, TOP_SPRING_Z]) 
            cylinder(h=FLOOR_Z, d=FLOOR_D);
        translate([0, 0, TOP_SPRING_Z + FLOOR_Z])
            cylinder(h=CASE_Z - LOWER_Z - TOP_SPRING_Z - FLOOR_Z - FLOOR_Z, d=BATTERY_D);

        translate([0, 0, -LOWER_Z]) {
            translate([-PASS_0_DX/2, -20, PASS_0_Z]) 
                cube(size=[PASS_0_DX, 40, PASS_0_DZ]);
            translate([-PASS_1_DX/2, -20, PASS_1_Z]) 
                cube(size=[PASS_1_DX, 40, PASS_1_DZ]);
        }
        if (CUTOUT) translate([-20, -20, 0]) cube(size=[40, 20, 100]);
    }
    translate([0, 0, SWITCH_RING_0_Z]) {
        difference() {
            cylinder(h=FLOOR_Z, d=CASE_D);
            cylinder(h=FLOOR_Z, d=SWITCH_RING_D);
            if (CUTOUT) translate([-20, -20, 0]) cube(size=[40, 20, 100]);
       }
    }
    translate([0, 0, SWITCH_RING_1_Z]) {
        difference() {
            cylinder(h=FLOOR_Z, d=CASE_D);
            cylinder(h=FLOOR_Z, d=SWITCH_RING_D);
            if (CUTOUT) translate([-20, -20, 0]) cube(size=[40, 20, 100]);
        }
    }
}
*/