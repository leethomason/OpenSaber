use <../shapes.scad>

$fn = 80;

difference() {
    tube(h=20, do=30, di=28);

    /*
    for(i=[0:3]) {
        Y = (i==2) ? 30/2 - 0.5 : 30/2 - 1.0;
        translate([0, Y, 4 + 4.0 * i]) rotate([-90, 0, 0]) cylinder(h=10, d=2);
    }
    */
    for(i=[0:3]) {
        if (i != 2)
            translate([0, 30/2 - 0.5, 4 + 3.0 * i]) 
                rotate([-90, 0, 0]) 
                    cylinder(h=10, d=2);
    }
}
