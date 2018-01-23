$fn = 60;

DY = 25.5;
DYV = [0, 25.5];
BAT_DX = 79;

H_BATTERY           = 68;
D_BATTERY           = 18.50 + 0.5;    // An 1850. Huh. 

CENTER = 2+DY/2;
SWITCH_BASE_X = 6;
SWITCH_BASE_Y = 3.5;
INSIDE_H = 18.5;

function bez(t) = [(1-t)*(1-t)*0 + 2*t*(1-t)*60 + t*t*120,
                   (1-t)*(1-t)*2 + 2*t*(1-t)*1.75 + t*t*9.5 ];

module mc(extend=false) {
    cube(size=[64, 4, 18.5]);
    translate([13.5, 4, 0]) cube(size=[53.5, 6.2, 18.5]);
    if (extend) translate([0, -5, 0]) cube(size=[64, 5, 18.5]);
}

module battery(extend=false) {
    translate([0, 0, D_BATTERY/2]) rotate([0, 90, 0]) {
        cylinder(h=H_BATTERY, d=D_BATTERY);
        if (extend) {
            translate([-D_BATTERY, -D_BATTERY/2, 0]) 
                cube(size=[D_BATTERY, D_BATTERY, H_BATTERY]);
        }
    }
}

rotate([0, 0, 30]) 
difference() {
    union() {
        color("green")
        linear_extrude(height = 2) {
            polygon(points=[
                [-20, 2], 
                bez(0), bez(0.2), bez(0.4), bez(0.5), bez(0.6), bez(0.8), bez(1.0),
                bez(1.0)+DYV, bez(0.8)+DYV, bez(0.6)+DYV, bez(0.5)+DYV, bez(0.4)+DYV, bez(0.2)+DYV, bez(0.0)+DYV,
                [-20,2+DY]]);
        }  
        
        // mc
        for(x=[0:3]) {
            translate([x*4, 2, 0]) cube(size=[2, 10, 10]);
        }      
        // battery
        for(x=[0:9]) {
            translate([x*4 + BAT_DX + 1, 6 + x/2, 0]) cube(size=[2, 21, 10]);
        }      

        // switch holder.
        translate([-18-SWITCH_BASE_X/2, 2+DY-SWITCH_BASE_Y, 0]) {
            cube(size=[SWITCH_BASE_X, SWITCH_BASE_Y, INSIDE_H/2 - SWITCH_BASE_X/2]);
            translate([0, -SWITCH_BASE_Y, 0]) 
                cube(size=[SWITCH_BASE_X, SWITCH_BASE_Y, INSIDE_H/2 + SWITCH_BASE_X/2]);
        }
        translate([-20, 2, 0]) cube(size=[2, DY, 4]);
    }
    translate([0, CENTER, 0]) cylinder(h=20, d=14.5);
    translate([2, 2, 0]) rotate([0, 0, 2]) mc(true);
    translate([BAT_DX, 19, 0]) rotate([0, 0, 6]) battery(true);
    
    for(x=[0:14]) {
        translate([11 + x*4, 16, 0]) cube(size=[2, 10, 3]);
    }
}

//translate([2, 2, 0]) rotate([0, 0, 2]) mc();
//translate([BAT_DX, 19, 0]) rotate([0, 0, 6]) battery(true);
