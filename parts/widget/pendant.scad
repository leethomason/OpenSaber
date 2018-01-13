$fn = 90;

CUTOUT = true;

CASE_D    =  9.5;
BATTERY_D =  7.9 + 0.5;
BATTERY_Z =  3.6;
LOWER_Z   = 17.0;          // The lower section. 12mm in the 2nd section.

SPRING_Z  = 2.0;
SPRING_D  = 6.0;
FLOOR_Z   = 1.0;
FLOOR_D   = 5.0;
LED_LIFT_Z = 2.0;

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

color("yellow") translate([0, 0, LED_LIFT_Z + FLOOR_Z + SPRING_Z]) {
    difference() {
        for(i=[0:2]) {
            translate([0, 0, i*BATTERY_Z]) {
                translate([0, 0, 1]) cylinder(h=BATTERY_Z-1, d=BATTERY_D);
                cylinder(h=1, d=BATTERY_D-1);
            }
        }
    }
}
