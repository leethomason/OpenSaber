use <../shapes.scad>

D_OUTER = 30.0;
D_INNER = 25.0;
D_TOOL = 3.175;

$fn = 80;

module arcCut(r, angle0, angle1)
{
    N = 5;
    for(i=[0:N-1]) {
        hull() {
            rotate([0, 0, angle0 + i*(angle1-angle0)/N]) 
                translate([r, 0, -1]) 
                    cylinder(h=10, d=D_TOOL);
            rotate([0, 0, angle0 + (i+1)*(angle1-angle0)/N]) 
                translate([r, 0, -1]) 
                    cylinder(h=10, d=D_TOOL);
        }
    }
}

difference() {
    union() {
        color("LightGray") tube(h=2.0, do=D_OUTER, di=D_INNER);
        color("Gainsboro") cylinder(h=2.0, d=D_INNER);
    }
    for(i=[0:2]) {
        arcCut(10, i*120 + 0, i*120 + 70);
        arcCut(5, i*120 + 12, i*120 + 58);
    }
}
