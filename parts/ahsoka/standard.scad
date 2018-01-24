use <../shapes.scad>

$fn = 60;
EPS = 0.001;
EPS2 = EPS * 2;

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

module hilt(h) {
    points=[
        [-14, 2], 
        bez(0), bez(0.2), bez(0.4), bez(0.5), bez(0.6), bez(0.8), bez(1.0),
        bez(1.0)+DYV, bez(0.8)+DYV, bez(0.6)+DYV, bez(0.5)+DYV, bez(0.4)+DYV, bez(0.2)+DYV, bez(0.0)+DYV,
        [-20,2+DY]];
    polygonXY(h, points);
}

module baffle(bridge=false, crystal = 7.5) {
    DY = 12;
    DX = 6;

    difference() 
    {
        cube(size=[2, DY, 14]);
        translate([-EPS, DY/2, INSIDE_H/2]) rotate([0, 90, 0]) cylinder(h=10, d=crystal);
        translate([-EPS, DY/2, INSIDE_H/2]) rotate([45, 0, 0]) cube(size=[20, 20, 20]);
    }

    if (bridge) color("green") {
        points = [[0, 4], [DX, 4+DX], [0, 4+DX]];
        translate([0, .5, 0]) polygonXZ(1.5, points);
        translate([0, DY - 1.5, 0]) polygonXZ(1.5, points);    
    }
}

rotate([0, 0, 0]) 
difference() {
    union() {
        intersection() {
            hilt(INSIDE_H);
            union() {
                // battery
                for(x=[0:9]) {
                    translate([x*4 + BAT_DX + 1, 4, 0]) cube(size=[2, 40, 8]);
                }      
                for(y=[0:2]) {
                    translate([BAT_DX - 10, 14 + y*4, 0]) 
                        polygonXZ(2, [[0,0], [12,0], [12,8], [8,8]]);
//                    cube(size=[8, 2, 8]);
                }

                // crystal holder
                for(x=[2:10]) {
                    rotate([0, 0, 2]) translate([16 + x*4, 14, 0]) 
                        baffle(bridge = x < 10, crystal = min(7.5, x*2));
                }
                for(r=[0:11]) {
                    S = 30;
                    translate([-1, CENTER, 0]) rotate([0, 0, -60 + r*30]) 
                        //translate([-1, -S/2, 0]) 
                            polygonYZ(2, [[0,0], [16,0], [12, 8], [0,8]]);
                            //cube(size=[2, S-4, 8]);
                }
            }
        }
        hilt(2);

        // switch holder.
        translate([-18-SWITCH_BASE_X/2, 2+DY-SWITCH_BASE_Y, 0]) {
            cube(size=[SWITCH_BASE_X, SWITCH_BASE_Y, INSIDE_H/2 - SWITCH_BASE_X/2]);
            translate([-2, 0, 0]) cube(size=[2, SWITCH_BASE_Y, 10]);
            translate([SWITCH_BASE_X, 0, 0]) cube(size=[2, SWITCH_BASE_Y, 10]);
        }
    }
    translate([0, CENTER, -EPS]) cylinder(h=20, d=14.5);
    translate([4, 3, -EPS]) rotate([0, 0, 2]) mc(true);
    translate([BAT_DX, 19, 0]) rotate([0, 0, 6]) battery(true);
    
    for(x=[0:14]) {
        rotate([0, 0, 2]) translate([14 + x*4, 15, -EPS]) cube(size=[2, 10, 5]);
    }
}

//translate([2, 2, 0]) rotate([0, 0, 2]) mc();
//translate([BAT_DX, 19, 0]) rotate([0, 0, 6]) battery(true);

//translate([-20, -20, 0]) baffle();
