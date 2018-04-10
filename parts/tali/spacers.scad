$fn = 60;
EPS = 0.01;
EPS2 = 2 * EPS;

H = 4;
OUTER = 32.0;

ROD_D = 3.5 + 0.2;
ROD_X = 0;
ROD_Y = 11;

CHANNEL_D = 5.8 + 0.2;
CHANNEL_X = -8.0;
CHANNEL_Y = 8.0;

difference() {
    cylinder(h=H, d=OUTER);
    translate([ROD_X, ROD_Y, -EPS]) cylinder(h=H + EPS2, d=ROD_D);
    translate([-ROD_X, -ROD_Y, -EPS]) cylinder(h=H + EPS2, d=ROD_D);
    translate([CHANNEL_X, CHANNEL_Y, -EPS]) cylinder(h=H + EPS2, d=CHANNEL_D);
}