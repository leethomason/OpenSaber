$fn=40;

EDGE  = 70;
STONE =  6;
PIN_X = 45.72;
PIN_Y = 17.78;
POST_BASE_D = 5;
POST_PIN_D  = 2.4;
POST_Z0 = STONE/2 + 6;
POST_Z1 = POST_Z0 + 2;
PLATE_X = 10;
PLATE_Y = 60;
BAT_X = 37;
BAT_Y = 30;
BAT_Z =  4.5;

module cubeBounds()
{
    translate([-EDGE/2, -EDGE/2, 0]) 
        cube(size=[EDGE, EDGE, EDGE]);
}

translate([-EDGE/2, -EDGE/2, 0]) 
    cube(size=[STONE, STONE, STONE]);
translate([EDGE/2 - STONE, -EDGE/2, 0]) 
    cube(size=[STONE, STONE, STONE]);
translate([-EDGE/2, EDGE/2 - STONE, 0]) 
    cube(size=[STONE, STONE, STONE]);
translate([EDGE/2 - STONE, EDGE/2 - STONE, 0]) 
    cube(size=[STONE, STONE, STONE]);
    
intersection() {
    cubeBounds();
    
     union() {
         rotate([0, 0, 45]) {
            translate([-STONE/2, -100, 0]) {
                cube(size=[STONE, 200, STONE/2]);
            }
         }
         rotate([0, 0, -45]) {
            translate([-STONE/2, -100, 0]) {
                cube(size=[STONE, 200, STONE/2]);
            }
        }
    }
}

hull() {
    translate([-PIN_X/2, -PIN_Y/2, 0])
        cylinder(d=POST_BASE_D, h=STONE/2);
    translate([PIN_X/2, -PIN_Y/2, 0])
        cylinder(d=POST_BASE_D, h=STONE/2);
    translate([-PIN_X/2, PIN_Y/2, 0])
        cylinder(d=POST_BASE_D, h=STONE/2);
    translate([PIN_X/2, PIN_Y/2, 0])
        cylinder(d=POST_BASE_D, h=STONE/2);
}


translate([-PIN_X/2, -PIN_Y/2, 0]) {
    cylinder(d=POST_BASE_D, h=POST_Z0);
    cylinder(d=POST_PIN_D, h=POST_Z1);
}
translate([PIN_X/2, -PIN_Y/2, 0]) {
    cylinder(d=POST_BASE_D, h=POST_Z0);
    cylinder(d=POST_PIN_D, h=POST_Z1);
}
translate([-PIN_X/2, PIN_Y/2, 0]) {
    cylinder(d=POST_BASE_D, h=POST_Z0);
    cylinder(d=POST_PIN_D, h=POST_Z1);
}
translate([PIN_X/2, PIN_Y/2, 0]) {
    cylinder(d=POST_BASE_D, h=POST_Z0);
    cylinder(d=POST_PIN_D, h=POST_Z1);
}

translate([-PLATE_X/2, -PLATE_Y/2, 0]) {
    cube(size=[PLATE_X, PLATE_Y, STONE/2]);
}

module pillar() {
    S0 = PLATE_X;
    S1 = S0 * 0.5;
    S2 = S0 * 1.6;
    H0 = 18;
    H1 = 26;

    hull() {
        translate([-S0/2, -S0/2, 0]) cube(size=[S0, S0, STONE/2]);
        translate([-S1/2, -S1/2, H0]) cube(size=[S1, S1, 0.1]);
    }    
    difference() 
    {
        hull() {
            translate([-S1/2, -S1/2, H0]) cube(size=[S1, S1, 0.1]);
            translate([-S2/2, -S1/2, H1]) cube(size=[S2, S1, 0.1]);
        }    
        translate([0, 10, 27]) rotate([90, 0, 0]) cylinder(h=20, d=9);
    }
}

translate([0, -PLATE_Y/2 + PLATE_X/2, 0]) {
    pillar();
}

translate([0, PLATE_Y/2 - PLATE_X/2, 0]) {
    pillar();
}

difference() {
    union() {
        translate([-BAT_X/2 - STONE/2, -BAT_Y/2 - STONE/2, 0])
            cube(size=[STONE, STONE, STONE/2 + BAT_Z]);
        translate([BAT_X/2 - STONE/2, -BAT_Y/2 - STONE/2, 0])
            cube(size=[STONE, STONE, STONE/2 + BAT_Z]);
        translate([-BAT_X/2 - STONE/2, BAT_Y/2 - STONE/2, 0])
            cube(size=[STONE, STONE, STONE/2 + BAT_Z]);
        translate([BAT_X/2 - STONE/2, BAT_Y/2 - STONE/2, 0])
            cube(size=[STONE, STONE, STONE/2 + BAT_Z]);
    }
    translate([-BAT_X/2, -BAT_Y/2, STONE/2]) 
        cube(size=[BAT_X, BAT_Y, BAT_Z]);
}