$fn = 60;

H = 2;
D = 5.5;
OUTER = 45;

difference() {
    hull() {
        translate([0,  0, 0]) cylinder(h=H, d=D);
        translate([OUTER+2, 0, 0]) cylinder(h=H, d=D);
        translate([0,  OUTER, 0]) cylinder(h=H, d=D);
        translate([OUTER+2, OUTER, 0]) cylinder(h=H, d=D);
    }
    translate([2,     2,    -1]) cylinder(h=H+2, d=D);
    translate([44,    2,    -1]) cylinder(h=H+2, d=D);
    translate([2,     2+41, -1]) cylinder(h=H+2, d=D);
    translate([44,    2+41, -1]) cylinder(h=H+2, d=D);
    
    // Test how big the cut is
    *translate([24, (2+41)/2, 0]) {
       cylinder(h=H, d=1.25 * 25.4 + 3.2*2);
    } 
    
    XSTART = -1;
    XEND = 3;
    YSTART = -1;
    YEND = 3;
    
    for(x=[XSTART:XEND]) {
        for(y=[YSTART:YEND]) {
            xAtEnd = (x == XSTART) || (x == XEND);
            yAtEnd = (y == YSTART) || (y == YEND);
            atEnd = xAtEnd && yAtEnd;
            if (!atEnd)
                translate([11+x*12, 11+y*12, -1]) cylinder(h=H+2, d=9);
        }
    }
}