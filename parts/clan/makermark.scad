use <../shapes.scad>

$fn = 80;

Z = 20;
STRIDE = 2.8;
D = 2.0;

difference() {
    tube(h=Z, do=30, di=28);

    for(i=[0:3]) {
        if (i != 2)
            translate([0, 30/2 - 0.5, Z/2 - STRIDE * 1.5 + STRIDE * i]) 
                rotate([-90, 0, 0]) 
                    cylinder(h=10, d=D);
    }
}
