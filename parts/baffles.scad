use <shapes.scad>

D_OUTER = 2;
D_THIN  = 3;
D_INNER = 6;

module curveBridge(h, y, z, low, high)
{
    EPS = 0.1;

    // a + b*t + c*t*t + d*t*t*t
    al = low[0];
    bl = low[1];
    cl = low[2];
    dl = low[3];
    ah = high[0];
    bh = high[1];
    ch = high[2];
    dh = high[3];

    t = z;
    points = [
        [-EPS, al],
        [0.1, al + 0.1*bl + 0.1*0.1*cl + 0.1*0.1*0.1*dl],
        [0.2, al + 0.2*bl + 0.2*0.2*cl + 0.2*0.2*0.2*dl],
        [0.3, al + 0.3*bl + 0.3*0.3*cl + 0.3*0.3*0.3*dl],
        [0.4, al + 0.4*bl + 0.4*0.4*cl + 0.4*0.4*0.4*dl],
        [0.5, al + 0.5*bl + 0.5*0.5*cl + 0.5*0.5*0.5*dl],
        [0.6, al + 0.6*bl + 0.6*0.6*cl + 0.6*0.6*0.6*dl],
        [0.7, al + 0.7*bl + 0.7*0.7*cl + 0.7*0.7*0.7*dl],
        [0.8, al + 0.8*bl + 0.8*0.8*cl + 0.8*0.8*0.8*dl],
        [0.9, al + 0.9*bl + 0.9*0.9*cl + 0.9*0.9*0.9*dl],
        [1.0+EPS, al + 1.0*bl + 1.0*1.0*cl + 1.0*1.0*1.0*dl],
        [1.0+EPS, ah + 1.0*bh + 1.0*1.0*ch + 1.0*1.0*1.0*dh],
        [0.9, ah + 0.9*bh + 0.9*0.9*ch + 0.9*0.9*0.9*dh],
        [0.8, ah + 0.8*bh + 0.8*0.8*ch + 0.8*0.8*0.8*dh],
        [0.7, ah + 0.7*bh + 0.7*0.7*ch + 0.7*0.7*0.7*dh],
        [0.6, ah + 0.6*bh + 0.6*0.6*ch + 0.6*0.6*0.6*dh],
        [0.5, ah + 0.5*bh + 0.5*0.5*ch + 0.5*0.5*0.5*dh],
        [0.4, ah + 0.4*bh + 0.4*0.4*ch + 0.4*0.4*0.4*dh],
        [0.3, ah + 0.3*bh + 0.3*0.3*ch + 0.3*0.3*0.3*dh],
        [0.2, ah + 0.2*bh + 0.2*0.2*ch + 0.2*0.2*0.2*dh],
        [0.1, ah + 0.1*bh + 0.1*0.1*ch + 0.1*0.1*0.1*dh],
        [-EPS, ah]
    ];

    multmatrix(m = [ [0, 0, 1, 0],
                     [0, y, 0, 0],
                     [z, 0, 0, 0],
                     [0, 0, 0, 1]
                ])
    linear_extrude(height = h) {
        polygon(points=points);
    }  	
}

module bridge2(d, dz)
{
    SPACE_I = 6.0;
    SPACE_O = 1.0;

    difference() {
        intersection() 
        {
            tube(h=dz, do=d - SPACE_O, di=d - SPACE_I);
            union() {
                translate([-d, d*0.15, 0]) {
                    polygonYZ(d*2, [
                        [0,0], [dz*2, dz*2], [dz*3.5, dz*1.5], [dz*2.5, 0]
                    ]);
                }        
                translate([-d, -d*0.32, 0]) {
                    polygonYZ(d*2, [[0,0], [dz*2, dz*2], [dz*3, 0]]);
                }        
            }
        }
        BATTERY = 19.0; // padded
        translate([0, d/2 - BATTERY/2, 0])
            cylinder(h=d, d=BATTERY);
    }
}

module bridge4UpperWall(d, dz)
{
    intersection() 
    {
        cylinder(h=dz, d=d - 1.0);
        union() {
            translate([-d/2 - 1.0, d*0.08, 0]) {
                rotate([0, 0, -35]) {
                    polygonYZ(2.0, [
                        [0,0], 
                        [dz*3.0, dz*2], 
                        [dz*3.0, dz*1.5], 
                        [dz*3.0, 0]
                    ]);
                }
            }        
        }
    }
}

module bridge4(d, dz)
{
    intersection() 
    {
        tube(h=dz, do=d - D_OUTER, di=d - D_INNER);
        translate([-d, -d*0.32, 0]) {
            polygonYZ(d*2, [[0,0], [dz*2, dz*2], [dz*2, 0]]);
        }        
    }
    bridge4UpperWall(d, dz);
    mirror([-1, 0, 0]) bridge4UpperWall(d, dz);
}

module bridge5(d, dz)
{
    EPS = 0.1;
    D_BATTERY = 18.50 + 0.5;    // An 1850. Huh. 
    difference() {
        intersection() 
        {
            tube(h=dz, do=d - 2.0, di=d - 8.0);
            union() {
                translate([-d, -d*0.35, -EPS]) {
                    polygonYZ(d*2, [[0,0], [dz*2, dz*2], [dz*2, 0]]);
                }        
                translate([-d, 0, 0]) {
                    polygonYZ(d*2, [
                        [0,0], [dz, dz+EPS*2], 
                        [d*0.4, dz+EPS*2], [d*0.25, 0]
                    ]);
                }        
            }
        }
        translate([0, d/2 - D_BATTERY/2, 0])
            cylinder(d=D_BATTERY + 1.0, h = dz+EPS);
    }
}

module bridge3(d, dz)
{
    difference() {
        union() {
            intersection() 
            {
                tube(h=dz, do=d - D_OUTER, di=d - D_INNER);
                union() {
                    translate([-d, 0, 0]) {
                        curveBridge(d*2, dz, dz, 
                            [-2.5, 1.0, 1.0, 0.0], 
                            [-1.5, 0.5, 2.0, 0.0]);
                    }
                    translate([-d, 0, 0]) {
                        curveBridge(d*2, dz, dz, 
                            [ 0.0, 1.0, 1.0, 0.0], 
                            [ 1.5, 0.0, 1.5, 0.0]);
                    }
                }
            }
            color("silver") 
            intersection() 
            {
                tube(h=dz, do=d - D_THIN, di=d - D_INNER);
                translate([-d, 0, 0]) {
                    curveBridge(d*2, dz, dz, 
                        [ 2.5, -1.0, -1.5, 0.0], 
                        [ 3.0, -1.0, 0.5, 0.0]);
                }
            }
            color("silver")
            intersection() 
            {
                tube(h=dz, do=d - D_THIN, di=d - D_INNER);
                translate([-d, 0, 0]) {
                    curveBridge(d*2, dz, dz, 
                        [ -0.8, -1.0, -0.5, -0.2], 
                        [ -0.2, -1.0, -0.1, 0.0]);
                }
            }
        }
        BATTERY = 19.0; // padded
        translate([0, d/2 - BATTERY/2, 0])
            cylinder(h=d, d=BATTERY);
        X = 19.0; // padded
        translate([-X/2, -d/2, 0])
            cube(size=[X, d/2, dz]);
    }
}

translate([0, 0, 0]) tube(h=4, do=32, di=26);
translate([0, 0, 8]) tube(h=4, do=32, di=26);
translate([0, 0, 16]) tube(h=4, do=32, di=26);
translate([0, 0, 24]) tube(h=4, do=32, di=26);
translate([0, 0, 32]) tube(h=4, do=32, di=26);

translate([0, 0, 4]) bridge3(32, 4);
translate([0, 0, 12]) bridge3(32, 4);
translate([0, 0, 20]) bridge3(32, 4);
translate([0, 0, 28]) bridge3(32, 4);


/*
translate([0, 0, 0]) cube(size=[1, 30, 4]);
translate([0, 0, 8]) cube(size=[1, 30, 4]);
translate([-1, 4, 4]) 
    curveBridge(0.8, 4, 4, [0, 1.0, 0.3, 0.1], 
                           [1, 1.0, 0.3, 0.1]);
translate([-1, 16, 4]) 
    curveBridge(0.8, 4, 4, [0, 1.0, 0.3, 0.1], 
                           [1, 1.0, 0.3, 0.1]);                           

translate([0, 16, 4]) 
    curveBridge(0.8, 4, 4, [0, -1.0, 0.3, 0.1], 
                           [0.6, -1.0, 0.3, 0.1]);
*/