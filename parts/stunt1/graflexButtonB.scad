use <../shapes.scad>

BUTTON_D = 5.0;

D0 = 5.2;
D1 = 7.8;
D2 = 10.7;
D3 = 13.7;
H1 = 6.3;
H2 = 3.5;

$fn = 80;

BOLT_H = 9.3 - 6;
HEAD_H = 4.2;
HEAD_D = 7.5;

difference() {
    union() {
        cylinder(h=H1, d=D1);
        translate([0, 0, H1 - H2]) cylinder(h=H2, d=D2);
        translate([0, 0, H1 - 1.0]) 
            intersection() {
                translate([-100, -100, 0])
                    cube(size=[200, 200, 100]);
                hull() 
                    rotate_extrude(convexity = 10)
                        translate([D3/2 - BUTTON_D/2, 0, 0])
                            circle(d=BUTTON_D);
            }
     }    
    cylinder(h=100, d = D0);
    translate([0, 0, BOLT_H]) cylinder(h=100, d=HEAD_D);
}

*color("red") translate([0, 0, BOLT_H]) cylinder(h=HEAD_H, d=7.0);
