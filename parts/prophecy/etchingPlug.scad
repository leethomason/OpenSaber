$fn = 90;

D_OUTER = 31.5;
D_INNER = 25.4;

difference() {
    union() {
        cylinder(d=D_INNER, h=40);
        difference() 
        {
            cylinder(d=D_OUTER, h=40); 
            rotate([45, 0, 0]) {
                translate([-40, -40, 14]) {
                    cube(size=[80, 80, 40]);
                }
            }
        }
    }
    translate([0, 0, 4]) cylinder(d=D_INNER-4, h=40);
}
