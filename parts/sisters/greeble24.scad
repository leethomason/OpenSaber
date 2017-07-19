include <dim.scad>

$fn = 80;

RF24_Z = 29;
RF24_X = 16;
BOLT_HEAD_D = 5;    // fixme
BOLT_HEAD_SPACE = 1;
BOLT_Z = BOLT_HEAD_D + BOLT_HEAD_SPACE * 2;
WALL_T = 1;


difference() {
    translate([-(RF24_X/2 + WALL_T), 0, 0]) {
        cube(size=[RF24_X + WALL_T*2,
                   D_SABER_OUTER / 2 + 5,
                   RF24_Z + BOLT_Z*2 + WALL_T*2]);
    }
    cylinder(d=D_SABER_OUTER, h=100);
}