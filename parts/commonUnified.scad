use <shapes.scad>

// TODO: unify the OledHolder and the PCB holder

Z_BATTERY           =  68;
Z_PADDED_BATTERY    =  Z_BATTERY + 1;
D_BATTERY           =  18.50 + 0.5;    // An 1850. Huh. 

D_SWITCH            =  12.5;     // actually 12, by thread.
D_SWITCH_SUPPORT    =  16;
DY_SWITCH            =  -2; //13.5;

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

D_M2                = 1.7;

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

module columnY(dx, dyFrom0, dz, diameter)
{
    intersection() {
        translate([0, 0, -dz/2]) cylinder(h=dz, d=diameter);
        translate([-dx/2, -diameter/2, -dz/2]) {
            cube(size=[dx, diameter/2 + dyFrom0, dz]);
        }
    }
}

/*
    The green-sky version of the key joint was large, but
    secure. Problematic is that it click together on the y
    axis so it's hard to run the wires.
*/
module cylinderKeyJoint(dz)
{
    X0 = 18;
    X1 = 10;
    Y  = 7;
    path = [
        [X0, 0], [X1, Y], [X1, -Y]
    ];
    polygonXY(dz, path);
    mirror([1,0,0]) polygonXY(dz, path);
}

module dotstarLED(n, dy)
{
    DOTSTAR_XZ = 5;
    DOTSTAR_PITCH = 7;

    for(i=[0:n-1]) {
        translate([-DOTSTAR_XZ/2, 0, DOTSTAR_PITCH*i - DOTSTAR_XZ/2]) {
            cube(size=[DOTSTAR_XZ, dy, DOTSTAR_XZ]);
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
    H_SPKR_METAL        =  4;
    D_SPKR_METAL        =  18; 
    H_SPKR_PLASTIC      =  4.6;
    D_SPKR_PLASTIC      =  20;

    color("yellow") {
        cylinder(h=H_SPKR_PLASTIC + EPS, d=D_SPKR_PLASTIC);
        translate([0, 0, H_SPKR_PLASTIC])
            cylinder(h=H_SPKR_METAL, d=D_SPKR_METAL);
    }
}

module speakerStd28(bass)
{
    H_SPKR        =  3.2;
    D_SPKR        =  28;
    X_NOTCH       =  6;
    Z_NOTCH       =  11.5;

    color("yellow") {
        cylinder(h=H_SPKR, d=D_SPKR);
        translate([-X_NOTCH/2, 0, 0])
            cube(size=[X_NOTCH, D_SPKR * 0.6, Z_NOTCH]);
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


module oneBaffleBottonRail(d, dz) 
{
    yMC = -yAtX(X_MC/2, d/2) + 0.5;

    intersection() {
        translate([0, 0, -EPS]) cylinder(h=dz*2 + EPS2, d=d);
        union() {
            translate([TROUGH_2 / 2, yMC - 10, -EPS])
                cube(size=[20, 10, dz*2 + EPS]);
            translate([X_MC / 2, yMC - 10, -EPS])
                cube(size=[20, 11, dz*2 + EPS]);
        }
    }    
}

TROUGH_0 = 10;
TROUGH_1 = 10;
TROUGH_2 = 14;
X_BRIDGE = X_MC/2+1;
T_BRIDGE = 1.6;

module oneBaffle(   d,
                    dz,
                    battery=true, 
                    mc=true,
                    useRods=true, 
                    bridge=true, 
                    mcSpace=false,
                    dExtra=0)
{
    yMC = -yAtX(X_MC/2, d/2) + 0.7;
    //yMC = -yAtX(X_MC/2, d/2) + 0.5;

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

        oneBaffleBottonRail(d, dz);
        mirror([1,0,0]) oneBaffleBottonRail(d, dz);
    }
}


// Resuable parts -----------------------------------
/*
    type = 
        "bass22"
        "std28"
 */
module speakerHolder(outer, dz, dzToSpkrBack, type)
{
    difference() {
        union() {
            if(type == "bass22") {
                difference() {
                    tube(h=dz, do=outer, di=18);
                    translate([0, 0, dzToSpkrBack])
                        speakerBass22();
                }
            }
            else if (type == "std28" || type == "bass28") {
                difference() {
                    tube(h=dz, do=outer, di=24);
                    translate([0, 0, dzToSpkrBack])
                        speakerStd28(type == "bass28");
                }
            }
            else
                echo("Error speaker type not specified.");
        }
        translate([-50, -outer, -EPS])
            cube(size=[100, outer - 6, 100]);    

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

module powerPortRing(outer, t, dz, dzToPort, portSupportToBack=false)
{    
    difference() {
        union() {
            tube(h=dz, do=outer, di=outer-t);
            intersection() {
                cylinder(h=dz, d=outer);
                if (portSupportToBack) {
                    translate([-50, outer/2 - DY_PORT, 0])
                        cube(size=[100, 50, D_PORT_SUPPORT/2 + dzToPort]);
                }
                else {
                    translate([-50, outer/2 - DY_PORT, dzToPort - D_PORT_SUPPORT/2])
                        cube(size=[100, 50, D_PORT_SUPPORT]);
                }
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

function zLenOfBaffles(n, dzButtress) = n * dzButtress * 2 - dzButtress;

module baffleMCBattery(outer, n, dzButtress, dFirst, dzFirst)
{
    for(i=[0:n-1]) {
        translate([0, 0, i*dzButtress*2]) 
            if (i==0) {
                // First baffle can "overflow" decause of
                // the larger diameter. Use an intersection()
                // to bring it in.
                intersection() {
                    cylinder(h=dzButtress*2, d=dFirst);
                    oneBaffle(outer, dzFirst, dExtra=dFirst - outer);
                }
            }
            else {
                oneBaffle(outer, dzButtress, bridge=(i < n-1));
            }
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

module pcbButtress()
{    
    BUTTRESS_T = 5;
    difference() {
        translate([0,0,-BUTTRESS_T/2])
            polygonXY(BUTTRESS_T, [[-2,0], [-2,-2], [14,-30], [14,0]]);
        translate([0, -50, 0]) rotate([-90, 0, 0]) cylinder(h=50, d=D_M2);
    } 
}

module pcbPillar() {
    translate([0, -50, 0]) 
        rotate([-90, 0, 0])
            difference() {
                cylinder(h=50, d1=8, d2=5);
                cylinder(h=50, d=D_M2);
            }
}

/*
    outer: diameter of the saber 
    t: thickness of the wall
    dz: length of the section
    dzToPCB: delta to where the pcb start
    dyPCB: y delta to PCB bottom.
    size[3]: outer size of the pcb
    mount: array of:
        x location, y location, "pillar" or "buttress"
*/
module pcbHolder(outer, t, dz, dzToPCB, dyPCB, size, mount)
{
    difference() 
    {
        union() {
            tube(h=dz, do=outer, di=outer-t);

            intersection() {
                cylinder(h=dz, d=outer);

                color("plum") union() {
                    for(m = mount) {
                        x = m[0];
                        z = m[1];
                        type = m[2];
                        translate([x, dyPCB, dzToPCB + z]) 
                        //translate([0, dyPCB, dzToPCB + z]) 
                        {
                            if (type == "pillar") {
                                pcbPillar();
                            }
                            else if (type == "buttress") {
                                shouldMirror = x < 0;
                                //echo("shouldMirror", shouldMirror);
                                if (shouldMirror)
                                    mirror([1,0,0])
                                        pcbButtress();
                                else
                                    pcbButtress();
                            }
                        }
                        
                    }
                }
            }
        }
        translate([-size[0]/2, dyPCB, dzToPCB]) 
            cube(size=[size[0], size[1], size[2]]);
    }
}
