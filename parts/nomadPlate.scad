use <shapes.scad>

IN_TO_MM = 25.4;
PLATE_H = 3.0;
PLATE_D = 6.0;

$fn=40;

module n883_holes(h=PLATE_H, d=PLATE_D)
{
   translate([0, 0, 0])            cylinder(h=h, d=d); 
   translate([0, 2.0*IN_TO_MM, 0]) cylinder(h=h, d=d); 
   translate([0, 5.0*IN_TO_MM, 0]) cylinder(h=h, d=d); 
   translate([0, 7.0*IN_TO_MM, 0]) cylinder(h=h, d=d); 

   translate([1.25*IN_TO_MM, 3.5*IN_TO_MM, 0]) cylinder(h=h, d=d); 

   translate([4.25*IN_TO_MM, 3.5*IN_TO_MM, 0]) cylinder(h=h, d=d); 

   translate([5.5*IN_TO_MM, 2.0*IN_TO_MM, 0]) cylinder(h=h, d=d); 
   translate([5.5*IN_TO_MM, 5.0*IN_TO_MM, 0]) cylinder(h=h, d=d); 

   translate([7.0*IN_TO_MM, 0, 0]) cylinder(h=h, d=d); 
   translate([7.0*IN_TO_MM, 7.0*IN_TO_MM, 0]) cylinder(h=h, d=d); 
}

module n883_centeredHoles(h=PLATE_H, d=PLATE_D) {
    translate([0.5*IN_TO_MM, 0.5*IN_TO_MM, 0]) n883_holes(h, d);
}

module arm(t) {
    TOP = 34;
    polygonXZ(t, [[25,0], [3*IN_TO_MM,0], [3*IN_TO_MM, TOP], [2*IN_TO_MM, TOP]]);
}

module slot(d, dx0, dx1) {
    hull() {
        translate([dx0, 0, 0]) cylinder(h=10, d=d);
        translate([dx1, 0, 0]) cylinder(h=10, d=d);
    }
}

module fixedPlate()
{
    T = 8;
    difference() {
        union() {
            translate([0, 2*IN_TO_MM, 0]) {
                cube(size=[3.0*IN_TO_MM, 4*IN_TO_MM, 5]);
            }
            translate([0, 2.5*IN_TO_MM - T/2, 0])
                arm(T);
            translate([0, 5.5*IN_TO_MM - T/2, 0])
                arm(T);
        }
        n883_centeredHoles(h=10, d=6.2);
        translate([3*IN_TO_MM + 4, 0, 20]) rotate([-90,0,0]) cylinder(h=200, d=40);

        translate([0, 4*IN_TO_MM, 0]) cylinder(h=20, d=60);
        hull() {
            translate([1.75*IN_TO_MM + 2, 4*IN_TO_MM + 18, 0]) cylinder(h=20, d=24);
            translate([1.75*IN_TO_MM + 5, 4*IN_TO_MM + 18, 0]) cylinder(h=20, d=24);
        }
        hull() {
            translate([1.75*IN_TO_MM + 2, 4*IN_TO_MM - 18, 0]) cylinder(h=20, d=24);
            translate([1.75*IN_TO_MM + 5, 4*IN_TO_MM - 18, 0]) cylinder(h=20, d=24);
        }
    }
}
   
module varPlate()
{
    T = 8;
    difference() {
        union() {
            translate([3.5*IN_TO_MM, 2*IN_TO_MM, 0]) {
                cube(size=[3.5*IN_TO_MM, 4*IN_TO_MM, 5]);
            }
            translate([6.5*IN_TO_MM, 2.5*IN_TO_MM - T/2, 0])
                mirror([1,0,0]) arm(T);
            translate([6.5*IN_TO_MM, 5.5*IN_TO_MM - T/2, 0])
                mirror([1,0,0]) arm(T);
        }
        n883_centeredHoles(h=10, d=6.2);
        translate([3.5*IN_TO_MM - 4, 0, 20]) rotate([-90,0,0]) cylinder(h=200, d=40);

        translate([4.75*IN_TO_MM, 4.0*IN_TO_MM, 0]) slot(6.2, -10, 14);
        translate([6*IN_TO_MM, 2.5*IN_TO_MM, 0]) slot(6.2, -10, 14);
        translate([6*IN_TO_MM, 5.5*IN_TO_MM, 0]) slot(6.2, -10, 14);

        translate([7*IN_TO_MM, 4*IN_TO_MM, 0]) cylinder(h=20, d=60);
        hull() {
            translate([4.5*IN_TO_MM + 5, 4*IN_TO_MM + 18, 0]) cylinder(h=20, d=24);
            translate([4.5*IN_TO_MM + 18, 4*IN_TO_MM + 18, 0]) cylinder(h=20, d=24);
        }
        hull() {
            translate([4.5*IN_TO_MM + 5, 4*IN_TO_MM - 18, 0]) cylinder(h=20, d=24);
            translate([4.5*IN_TO_MM + 18, 4*IN_TO_MM - 18, 0]) cylinder(h=20, d=24);
        }
    }
}

//n883_centeredHoles();
//fixedPlate();
varPlate();