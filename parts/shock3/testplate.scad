include <dim.scad>
use <../shapes.scad>

$fn = 80;
EPS = 0.01;

difference() {
    intersection() {
        hull() {
            R = 3.175 / 2;
            DZ = M_INSET_END - M_INSET_START;
            translate([DX_INSET/2 - R, 0, R]) rotate([-90, 0, 0]) cylinder(h=20, r=R);
            translate([-DX_INSET/2 + R, 0, R]) rotate([-90, 0, 0]) cylinder(h=20, r=R);
            translate([DX_INSET/2 - R, 0, DZ - R]) rotate([-90, 0, 0]) cylinder(h=20, r=R);
            translate([-DX_INSET/2 + R, 0, DZ - R]) rotate([-90, 0, 0]) cylinder(h=20, r=R);
        }
        translate([0, -R_OUTER + STOCK_H, 0]) cylinder(h=100, d=D_OUTER, $fn=80);
    }
    translate([0, -EPS, M_BOLT - M_INSET_START]) rotate([-90, 0, 0]) cylinder(h=100, d=4.4);
    translate([0, STOCK_H - 1.5, M_BOLT - M_INSET_START]) rotate([-90, 0, 0]) cylinder(h=100, d=8.0);
    
    // translate([0, -EPS, M_PORT_CENTER - M_INSET_START]) rotate([-90, 0, 0]) cylinder(h=100, d=8.0); 
    // translate([0, STOCK_H - 3, M_PORT_CENTER - M_INSET_START]) rotate([-90, 0, 0]) cylinder(h=100, d=11.0); 
    translate([0, -EPS, M_PORT_CENTER - M_INSET_START]) rotate([-90, 0, 0]) cylinder(h=100, d=11.0); 

    translate([0, -EPS, M_SWITCH_CENTER - M_INSET_START]) rotate([-90, 0, 0]) cylinder(h=100, d=BUTTON_HEAD_D + 0.2); 
    hull() {
        translate([0, STOCK_H - 2.0, M_SWITCH_CENTER - M_INSET_START]) rotate([-90, 0, 0]) cylinder(h=100, d=8.0); 
        DZ_EDGE = M_PORT_CENTER - 11.0/2;
        translate([0, STOCK_H - 2.0, M_INSET_END - DZ_EDGE - 8.0/2]) rotate([-90, 0, 0]) cylinder(h=100, d=8.0); 
    }
    //translate([0, -10, 0]) cylinder(h=100, d=24);
}

echo("Bolt", M_BOLT - M_INSET_START);
echo("Port", M_PORT_CENTER - M_INSET_START);
echo("Switch", M_SWITCH_CENTER - M_INSET_START);

//color("olive") translate([0, -R_OUTER + STOCK_H, EPS]) tube(h=DZ_INSET - EPS*2, do=D_OUTER-EPS, di=D_INNER+EPS);