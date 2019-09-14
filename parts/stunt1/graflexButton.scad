use <../shapes.scad>

D = 17.0;
BUTTON_D = 6.0;

H_UNDER = 2.0;
D2_UNDER = 12.9;
D1_UNDER = 8.0;
D0_UNDER = 4.7;

$fn = 80;

BOLT_H = 7.0 - 4.2;
HEAD_H = 2.5;   // rounded head, 8-32
HEAD_D = 7.8;

difference() {
    union() {
        translate([0, 0, H_UNDER])
        intersection() {
            translate([-100, -100, 0])
                cube(size=[200, 200, 100]);
            hull() 
                rotate_extrude(convexity = 10)
                    translate([D/2 - BUTTON_D/2, 0, 0])
                        circle(d=BUTTON_D);
        }

        {
            tube(h=H_UNDER, do=D, di=D2_UNDER);
            tube(h=H_UNDER, do=D1_UNDER, di=D0_UNDER);
        }
    }    

    cylinder(h=100, d = D0_UNDER);
    translate([0, 0, BOLT_H]) cylinder(h=100, d=HEAD_D);
}

*color("red") translate([0, 0, BOLT_H]) cylinder(h=HEAD_H, d=HEAD_D);
