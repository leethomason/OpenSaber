$fn = 180;

H     =  5.0;
OUTER = 21.2;
INNER = 18.5;

EPS = 0.01;
EPS2 = 2 * EPS;

difference() {
    cylinder(h=H, d=OUTER);
    translate([0, 0, -EPS]) cylinder(h=H + EPS2, d=INNER);
}