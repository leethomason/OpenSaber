use <shapes.scad>

UNIT = 2.54;
H = 8.6;
TEETH = 6.1;
TEETH_CAP = 1.0;
R_BOLT = UNIT * 3;

D_TUBE_INNER = (R_BOLT - 1.5) * 2;
D_TUBE_OUTER = (R_BOLT + 1.5) * 2 + 2.0;

$fn = 120;

module lock(h, cap, angleEps)
{
    x = cos(15 - angleEps);
    y = sin(15 - angleEps);
    M = 20;

    for(r=[0:5]) {
        rotate([0, 0, 60*r]) {
            if (!cap) {
                polygonXY(h=h, points=[
                    [0,0], [x*M, -y*M], [x*M, y*M]
                ]);
            }
            else {
                hull() {
                    polygonXY(h=0.1, points=[
                        [0,0], [x*M, -y*M], [x*M, y*M]
                    ]);
                    translate([0, 0, h-0.1]) polygonXY(h=0.1, points=[
                        [0,0], [x*M, -y*M + 1], [x*M, y*M - 1]
                    ]);
                }
            }
        }
    }
}

*color("gray") {
    translate([-UNIT*2, -UNIT/2, 0]) cube(size=[UNIT*4, UNIT, H]);
    translate([-UNIT, -1.5*UNIT, 0]) cube(size=[UNIT*2, UNIT*3, 5]);    // pcb wires
}

ANGLE = 0;  // outside
//ANGLE = 0.5; // inside

difference() {
    union() {
        tube(h=H - TEETH - TEETH_CAP, di=D_TUBE_INNER, do=D_TUBE_OUTER);
        intersection() {
            tube(h=100, di=D_TUBE_INNER, do=D_TUBE_OUTER);
            union() {
                translate([0, 0, H - TEETH - TEETH_CAP]) 
                    lock(TEETH + TEETH_CAP, false, ANGLE);
                translate([0, 0, H]) lock(TEETH_CAP, true, ANGLE);
            }
        }
    }
    translate([R_BOLT, 0, 0]) cylinder(h=100, d=1.8);
    translate([-R_BOLT, 0, 0]) cylinder(h=100, d=1.8);
}


echo("Outer D", (R_BOLT + 1.5) * 2);