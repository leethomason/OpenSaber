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

/*
    The green-sky version of the key joint was large, but
    secure. Problematic is that it click together on the y
    axis so it's hard to run the wires.
*/
module cylinderKeyJoint(dz)
{
    X0 = 10;
    X = 30;
    Y = 20;
    path = [
        [X0, 0], [X, Y], [X, -Y]
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

module speakerStd28()
{
    H_SPKR        =  4;       // FIXME: verify
    D_SPKR        =  28;

    color("yellow") {
        cylinder(h=H_SPKR, d=D_SPKR);
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

// Local origin is the bottom center of the board, back edge.
// 'outer' is the size of the board.
// 'mount' is the dimensions of the just the mounting holes.
// 'heightY' can be quite large, if modelling displays, etc.
// currently assumes 2mm.
module pcb(outerX, outerZ, mountX, mountZ, heightY)
{
    MOUNT_HOLE_D = 2;

    translate([-outerX/2, 0, 0]) {
        cube(size=[outerX, heightY, outerZ]);
    }
    translate([-mountX/2, -4, (outerZ - mountZ)/2])
        rotate([-90, 0, 0])
            cylinder(h=8 + heightY, d=MOUNT_HOLE_D);
    translate([mountX/2, -4, (outerZ - mountZ)/2])
        rotate([-90, 0, 0])
            cylinder(h=8 + heightY, d=MOUNT_HOLE_D);
    translate([-mountX/2, -4, outerZ - (outerZ - mountZ)/2])
        rotate([-90, 0, 0])
            cylinder(h=8 + heightY, d=MOUNT_HOLE_D);
    translate([mountX/2, -4, outerZ - (outerZ - mountZ)/2])
        rotate([-90, 0, 0])
            cylinder(h=8 + heightY, d=MOUNT_HOLE_D);
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
            else if (type == "std28") {
                difference() {
                    tube(h=dz, do=outer, di=24);
                    translate([0, 0, dzToSpkrBack])
                        speakerStd28();
                }
            }
            else
                echo("Error speaker type not specified.");
        }
        translate([-50, -outer, -EPS])
            cube(size=[100, outer - 5, 100]);    

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

module pcbColumn()
{
    translate([0,0,-3]) {
        polygonXY(6, [[-2,0], [-2,-2], [14,-20], [14,0]]);
    } 
}

/*
    outer: diameter of the saber 
    t: thickness of the wall
    dz: length of the section
    dzToPCB: delta to where the pcb start
    dyPCB: y delta to PCB bottom.
    size[3]: outer size of the pcb
    mount[3]: inner size to position the mounting holes.
              y should be zero.
*/
module pcbHolder(outer, t, dz, dzToPCB, dyPCB, size, mount)
{
    xm = size[0] - mount[0];
    zm = size[2] - mount[2];

    difference() 
    {
        union() {
            tube(h=dz, do=outer, di=outer-t);

            intersection() {
                cylinder(h=dz, d=outer);

                color("plum") union() {
                    translate([ size[0]/2 - xm/2, dyPCB, dzToPCB + zm/2 ])
                        pcbColumn();
                    mirror([1,0,0])
                    translate([ size[0]/2 - xm/2, dyPCB, dzToPCB + zm/2 ])
                        pcbColumn();
                    translate([ size[0]/2 - xm/2, dyPCB, dzToPCB + size[2] - zm/2 ])
                        pcbColumn();
                    mirror([1,0,0])
                    translate([ size[0]/2 - xm/2, dyPCB, dzToPCB + size[2] - zm/2 ])
                        pcbColumn();
                }
            }
        }
        translate([0, dyPCB, dzToPCB]) 
            pcb(size[0], size[2], mount[0], mount[2], size[1]);
        //translate([0, -4, dzToOled + 6])
        //    cube(size=[50, 50, OLED_DISPLAY_W - 3]);
    }
}
