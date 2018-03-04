use <shapes.scad>

Z_BATTERY           =  68;
Z_PADDED_BATTERY    = Z_BATTERY + 1;
D_BATTERY           =  18.50 + 0.5;    // An 1850. Huh. 

D_SWITCH            =  12.5;     // actually 12, by thread.
D_SWITCH_SUPPORT    =  16;
DY_SWITCH            =  -2; //13.5;

H_SPKR_METAL        =  4;
D_SPKR_METAL        =  18; 
H_SPKR_PLASTIC      =  4.6;
D_SPKR_PLASTIC      =  20;

D_PORT              =  7.9;
D_PORT_SUPPORT      =  12;
H_PORT              =  16;
DY_PORT             =  6;

X_MC                =  18.5;
Y_MC                =   9.0;
Z_MC                =  71.0;     // includes SD
DY_MC               = -12.5;

X_CRYSTAL           =  11;
Y_CRYSTAL           =   8.5;
Z_CRYSTAL           =  30.0;

OLED_R_DISPLAY_THREAD = 0.8; // M2 bolt, 2mm head
OLED_DEPTH_DISPLAY_THREAD = 4;
OLED_DISPLAY_W           = 23 + 1;
OLED_DISPLAY_L           = 32 + 1;
OLED_DISPLAY_MOUNT_W     = 17;
OLED_DISPLAY_MOUNT_L     = 26;
OLED_DY                  = -4;  // offset how far the holes/solder points
                                // should be under the case. mesaured from y=0 on model.

EPS = 0.01;
EPS2 = EPS * 2;

// Math ------------
OLED_DISPLAY_INNER_W = (OLED_DISPLAY_W - OLED_DISPLAY_MOUNT_W)/2;
OLED_DISPLAY_INNER_L = (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L)/2;

function yAtX(x, r) = sqrt(r*r - x*x);

// Utilities, shapes. ------------------------
module capsule(theta0, theta1, r=2)
{
    hull() {
        rotate([-90, -0, theta0]) cylinder(h=20, r=r);
        rotate([-90, -0, theta1]) cylinder(h=20, r=r);
    }
}

module columnY(dx, dy, dz, biasX=0, biasZ=0)
{
    translate([-dx/2, 0, -dz/2]) {
        hull() {
            cube([dx, EPS, dz]);
            translate([biasX, dy, biasZ]) cube([dx, EPS, dz]);
        }
    }
}

// Physical components. ----------------------
module battery(outer) {
    color("yellow") translate([0, outer/2 - D_BATTERY/2, 0]) {
        cylinder(d=D_BATTERY, h = Z_BATTERY + EPS2);
    }
}

module switch(outer, extend=false)
{
    D_OUTER_TOP = 13.8;
    D_INNER_TOP = 11.0;
    H_TOP       =  1.5;
    H_BODY      = 18;   // approx. connections below.

    color("yellow") {
        rotate([-90, 0, 0]) {
            translate([0, 0, outer/2 + DY_SWITCH]) {
                if (extend) {
                    cylinder(h=H_TOP+10, d=D_OUTER_TOP+1);
                }
                else {
                    cylinder(h=H_TOP, d1=D_OUTER_TOP+1, d2=D_INNER_TOP);
                }
                translate([0, 0, -H_BODY]) {
                    cylinder(h = H_BODY + EPS, d=D_SWITCH);
                }            
            }
        }
    }
}


module mc(cutoutLow=false)
{
    color("aqua") {
        translate([-X_MC/2, 0, 0])
            cube(size=[X_MC, Y_MC, Z_MC]);
    }
}

module crystal(dx=X_CRYSTAL, dy=Y_CRYSTAL, dz=Z_CRYSTAL)
{
    TAPER = 5;
    color("pink") scale([1.0, dy/dx, 1.0]) {
        cylinder(d=dx, h=dz - TAPER);
        translate([0, 0, dz - TAPER])
            cylinder(h=TAPER, d1 = dx, d2=dx *0.7);
    }
}

module port(extend=false)
{
    H = extend ? 30 : 21;
    rotate([-90, 0, 0]) {
        cylinder(h=H, d=D_PORT);
    }
}

module speakerBass22()
{
    color("yellow") {
        cylinder(h=H_SPKR_PLASTIC + EPS, d=D_SPKR_PLASTIC);
        translate([0, 0, H_SPKR_PLASTIC])
            cylinder(h=H_SPKR_METAL, d=D_SPKR_METAL);
    }
}

module oledDisplayBolt() 
{
    // Pins are challenging to print small & accurate.
    // Trying holes for M2 bolts instead.
    translate([0, -OLED_DEPTH_DISPLAY_THREAD, 0]) {
        rotate([-90, 0, 0]) cylinder(r=OLED_R_DISPLAY_THREAD, h=20);
    }
}

module oledDisplayBolts()
{
    translate([OLED_DISPLAY_INNER_W, 0, OLED_DISPLAY_INNER_L])                            
        oledDisplayBolt();
    translate([OLED_DISPLAY_W - OLED_DISPLAY_INNER_W, 0, OLED_DISPLAY_INNER_L])                 
        oledDisplayBolt();
    translate([OLED_DISPLAY_W - OLED_DISPLAY_INNER_W, 0, OLED_DISPLAY_L - OLED_DISPLAY_INNER_L])    
        oledDisplayBolt();
    translate([OLED_DISPLAY_INNER_W, 0, OLED_DISPLAY_L - OLED_DISPLAY_INNER_L])                
        oledDisplayBolt();
}


module oledDisplay()
{
    color("violet") translate([-OLED_DISPLAY_W/2, 0, 0]) {
        cube(size=[OLED_DISPLAY_W, 10, OLED_DISPLAY_L]);
        oledDisplayBolts();
        
        translate([0, -1, 6]) {
            cube(size=[OLED_DISPLAY_W, 1, OLED_DISPLAY_L - 12]);
        }
    }
    color("plum") translate([0, 0, 0]) {
        X_CABLE = 12;
        Y_CABLE = 2;
        Z_CABLE = 3 + 1;    // cheat so we don't get a super thin piece in the locking ring.
        translate([-X_CABLE/2, -Y_CABLE, -Z_CABLE]) {
            cube(size=[X_CABLE, 10, Z_CABLE + 7]);
        }
    }    
}


// Intermediate parts -----------------

module baffleHalfBridge(dz, t)
{
    polygonYZ(t, [[0,0], [dz*2, dz*2], [dz*2, 0]]);
}


module oneBaffle(   d,
                    dz,
                    battery=true, 
                    mc=true,
                    useRods=true, 
                    bridge=true, 
                    mcSpace=false,
                    dExtra=0)
{
    TROUGH_0 = 10;
    TROUGH_1 = 10;
    TROUGH_2 = 14;
    X_BRIDGE = X_MC/2+1;
    T_BRIDGE = 1.6;

    yMC = -yAtX(X_MC/2, d/2) + 0.5;

    difference() {
        cylinder(h=dz, d=d + dExtra);
        if (battery)
            translate([0, d/2 - D_BATTERY/2, -EPS]) 
                battery();
        if (mc) {
            translate([0, yMC, -EPS]) 
                mc();
        }
        if (mcSpace)
            translate([-X_MC/2, DY_MC, -EPS])
                cube(size=[X_MC, 20, dz+EPS2]);

        translate([-TROUGH_0/2, 0, -EPS]) 
            cube(size=[TROUGH_0, 30, dz + EPS2]);
        translate([-TROUGH_1/2, -5, -EPS]) 
            cube(size=[TROUGH_1, 5, dz + EPS2]);
        translate([-TROUGH_2/2, -20, -EPS])
            cube(size=[TROUGH_2, 15, dz + EPS2]);
    }

    if (bridge) {
        yBridge = yAtX(X_BRIDGE+T_BRIDGE, d/2);

        translate([X_BRIDGE, -yBridge, dz]) 
            baffleHalfBridge(dz, T_BRIDGE);        
        mirror([1, 0, 0]) translate([X_BRIDGE, -yBridge, dz]) 
            baffleHalfBridge(dz, T_BRIDGE);

        translate([X_BRIDGE, yBridge - dz*2, dz]) 
            baffleHalfBridge(dz, T_BRIDGE);        
        mirror([1, 0, 0]) translate([X_BRIDGE, yBridge - dz*2, dz]) 
            baffleHalfBridge(dz, T_BRIDGE);

        intersection() {
            translate([0, 0, -EPS]) cylinder(h=dz*2 + EPS2, d=d);
            union() {
                translate([TROUGH_2/2, yMC - 5, -EPS])
                    cube(size=[TROUGH_2, 5, dz*2 + EPS]);
                mirror([1,0,0]) translate([TROUGH_2/2, yMC - 5, -EPS])
                    cube(size=[TROUGH_2, 5, dz*2 + EPS]);
            }
        }
    }
}


// Resuable parts -----------------------------------
module speakerHolder(outer, dz, dzToSpkrBack)
{
    difference() {
        tube(h=dz, do=outer, di=D_SPKR_METAL);
        translate([0, 0, dzToSpkrBack])
            speakerBass22();
        translate([-50, -outer, -EPS])
            cube(size=[100, outer - 4, 100]);    

        THETA = 14;
        for(r=[0:2]) {
            rotate([0, 0, -50 + r*50]) translate([0, 0, dzToSpkrBack]) {
                for(x=[0:6]) {
                    tx = 3 * x + dzToSpkrBack;
                    if (tx + 5 <= dz) {
                        translate([0, 0, tx])  
                            capsule(-THETA, THETA, 1);
                    }
                }
            }
        }    
    }
}

module powerPortRing(outer, t, dz, dzToPort)
{    
    difference() {
        union() {
            tube(h=dz, do=outer, di=outer-t);
            intersection() {
                cylinder(h=dz, d=outer);
                translate([-50, outer/2 - DY_PORT, dzToPort - D_PORT_SUPPORT/2])
                    cube(size=[100, 50, D_PORT_SUPPORT]);
            }
        }
        translate([0, 0, dzToPort]) {
            port(true);
        }
    }
}


module switchRing(outer, t, dz, dzToSwitch)
{
    difference() {
        union() {
            tube(h=dz, do=outer, di=outer-t);
            intersection() {
                cylinder(h=dz, d=outer);
                translate([-50, outer/2 - 6, dzToSwitch - D_SWITCH_SUPPORT/2])
                    cube(size=[100, 50, D_SWITCH_SUPPORT]);
            }
        }
        translate([0, 0, dzToSwitch]) {
            switch(outer, true);
        }
    }
    *translate([0, 0, dzToSwitch]) {
        switch(outer, false);
    }
}


function nBafflesNeeded(dzButtress) = ceil(Z_PADDED_BATTERY / (dzButtress*2));

module baffleMCBattery(d, n, dzButtress, dFirst, dzFirst)
{
    for(i=[0:n-1]) {
        translate([0, 0, i*dzButtress*2]) 
            if (i==0)
                oneBaffle(d, dzFirst, dExtra=dFirst - d);
            else
                oneBaffle(d, dzButtress, bridge=(i < n-1));
    }
}


module oledColumn(outer)
{
    translate([0,0,-2]) {
        polygonXY(4, [[-2,0], [-2,-2], [14,-20], [14,0]]);
    } 
}


module oledHolder(outer, t, dz, dzToOled)
{
    yDisplay = yAtX((OLED_DISPLAY_W - OLED_DISPLAY_MOUNT_W)/2, outer/2) + OLED_DY;

    difference() 
    {
        union() {
            tube(h=dz, do=outer, di=outer-t);

            intersection() {
                cylinder(h=dz, d=outer);

                color("plum") union() {
                    translate([OLED_DISPLAY_W/2 - OLED_DISPLAY_INNER_W, yDisplay, dzToOled + OLED_DISPLAY_INNER_L])
                        oledColumn();
                    translate([-OLED_DISPLAY_W/2 + OLED_DISPLAY_INNER_W, yDisplay, dzToOled + OLED_DISPLAY_INNER_L])
                        mirror([1,0,0]) oledColumn();
                    translate([OLED_DISPLAY_W/2 - OLED_DISPLAY_INNER_W, yDisplay, dzToOled + OLED_DISPLAY_L - OLED_DISPLAY_INNER_L])
                        oledColumn();
                    translate([-OLED_DISPLAY_W/2 + OLED_DISPLAY_INNER_W, yDisplay, dzToOled + OLED_DISPLAY_L - OLED_DISPLAY_INNER_L])
                        mirror([1,0,0]) oledColumn();                    
                }
            }
        }
        translate([0, yDisplay, dzToOled]) 
            oledDisplay();
        translate([0, -4, dzToOled + 6])
            cube(size=[50, 50, OLED_DISPLAY_W - 3]);
    }
}