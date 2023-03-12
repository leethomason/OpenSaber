use <shapes.scad>
use <baffles.scad>

Z_BATTERY_18650     = 65 + 4;
Z_BATTERY_18500     = 50 + 5;
D_BATTERY           = 18.50 + 0.5;    // An 1850. Huh. 

D_SWITCH            =  12.5;     // actually 12, by thread.
D_SWITCH_SUPPORT    =  16;
DY_SWITCH            =  -2;

D_PORT              =  7.9;
D_PORT_SUPPORT      =  12;
H_PORT              =  16;

INCHES_TO_MM        = 25.4;
X_MC                =   0.7 * INCHES_TO_MM;
Y_MC                =   9.0;
Z_MC                =   2.2 * INCHES_TO_MM + 0.1;
DY_MC               = -12.5;

X_CRYSTAL           =  11;
Y_CRYSTAL           =   8.5;
Z_CRYSTAL           =  30.0;

D_M2                = 1.7;

EPS = 0.01;
EPS2 = EPS * 2;

// Math ------------

function yAtX(x, r) = sqrt(r*r - x*x);

function isSmall(d) = d < 29.5;

// Utilities, shapes. ------------------------
module capsule(theta0, theta1, r=2, _mirror=false)
{
    hull() {
        rotate([-90, -0, theta0]) cylinder(h=20, r=r);
        rotate([-90, -0, theta1]) cylinder(h=20, r=r);
    }
    if (_mirror == true) hull() {
        rotate([-90, -0, 180 + theta0]) cylinder(h=20, r=r);
        rotate([-90, -0, 180 + theta1]) cylinder(h=20, r=r);
    }
}

module triCapsule(theta0, theta1, r=2, _mirror=false)
{
    theta0 = theta0 + 90;
    theta1 = theta1 + 90;

    hull() {
        rotate([0, 0, theta0]) 
            polygonYZ(100, [[0, -r], [-r, 0], [0, r]]);
        rotate([0, 0, theta1]) 
            polygonYZ(100, [[0, -r], [r, 0], [0, r]]);
    }
    if (_mirror == true) hull() {
        mirror([1,0,0]) rotate([0, -0, theta0]) 
            polygonYZ(100, [[0, -r], [-r, 0], [0, r]]);
        mirror([1,0,0]) rotate([0, -0, theta1]) 
            polygonYZ(100, [[0, -r], [r, 0], [0, r]]);
    }
}

module columnY(dx, dyFrom0, dz, diameter, baseDX=0, baseDZ=0)
{
    intersection() {
        translate([0, 0, -dz/2]) cylinder(h=dz, d=diameter);
        hull() {
            translate([-dx/2, -diameter/2, -dz/2]) {
                cube(size=[dx, diameter/2 + dyFrom0, dz]);
            }
            translate([-(dx + baseDX)/2, -diameter/2, -(dz + baseDZ)/2]) {
                cube(size=[dx + baseDX, 1, dz + baseDZ]);
            }
        }
    }
}

/*
    The green-sky version of the key joint was large, but
    secure. Problematic is that it clicked together on the y
    axis so it's hard to run the wires. The jJoint=true version
    is by far the more successful design.
*/
module keyJoint(dz, do, di, slot, angle=0)
{
    DZ = dz;
    YTRIM = slot ? 0.6 : 0.0;
    ZTRIM = slot ? 0.4 : 0.0;
    T = do - di;

    intersection() {
        if (slot) {
            tube(h=DZ+2, di=di, do=do+1);
        }
        else {
            OVERLAP = dz * 0.75;
            difference() {
                union() {
                    cylinder(h=dz-OVERLAP, d=do);
                    translate([0, 0, dz-OVERLAP]) cylinder(h=OVERLAP, d1=do, d2=do-(T*0.2));
                }
                translate([0, 0, -EPS]) cylinder(h=100, d=di);
            }
        }

        union() {
            rotate([0, 0, angle]) 
                tjoint(dz, do, di, YTRIM, ZTRIM, slot);
            mirror([-1, 0, 0 ]) rotate([0, 0, angle]) 
                tjoint(dz, do, di, YTRIM, ZTRIM, slot);
        }
    }
}

/* 
    A simple bridge at 45 degrees for
    unsupported printing. Made to hold
    switches, boards, etc. 
*/
module simpleBridge(diameter, dyToTop, yThickness, dz, addWidth=0, flatFill=false)
{
    L = 50;
    delta = yThickness * 1.2;

    // Writing this without the 'if' enclosing the PATH
    // declaration revealed all sorts of interesting
    // OpenScad bugs.
    //
    if (flatFill) {
        PATH = [
            [-L, dyToTop - yThickness - L],
            [0, dyToTop - yThickness],
            [L, dyToTop - yThickness - L],
            [L, dyToTop],
            [delta + addWidth, dyToTop],
            [-delta - addWidth, dyToTop],
            [-L, dyToTop]
        ];
        translate([0, 0, -dz/2]) {
            intersection() {
                cylinder(h=dz, d=diameter);
                polygonXY(dz, PATH);
            }
        }
    }
    else {
        PATH = [
            [-L, dyToTop - yThickness - L],
            [0, dyToTop - yThickness],
            [L, dyToTop - yThickness - L],
            [L, dyToTop - L],
            [delta + addWidth, dyToTop],
            [-delta - addWidth, dyToTop],
            [-L, dyToTop - L]
        ];
        translate([0, 0, -dz/2]) {
            intersection() {
                cylinder(h=dz, d=diameter);
                polygonXY(dz, PATH);
            }
        }
    }
}

module bridge(dx, dy, dz, inset=0)
{
    h1 = dx * 0.57;
    h2 = dx * 0.65; 
    pillar = dx * 0.20;
    
    PATH = [ 
        [-dx/2, dy], 
        [-dx/2 + h1, dy],
        [-dx/2, dy-h2]
    ];

    INSET = [
        [-dx/2 - EPS, dy - inset],
        [-dx/2 - EPS, dy + EPS],
        [-dx/2 + inset, dy + EPS]
    ];

    intersection() 
    {
        translate([-dx/2, 0, 0]) cube(size=[dx, dy, dz]);
        difference() {
            union() {
                polygonXY(dz, PATH);
                mirror([1,0,0]) polygonXY(dz, PATH);

                translate([-dx/2, 0, 0]) cube(size=[pillar, dy, dz]);
                mirror([1,0,0]) translate([-dx/2, 0, 0]) cube(size=[pillar, dy, dz]);
            }
            if (inset > 0) {
                translate([0, 0, -EPS]) {
                    polygonXY(dz + EPS2, INSET);
                    mirror([1,0,0]) polygonXY(dz + EPS2, INSET);
                }
            }
        }
    }
}

// Physical components. ----------------------

function zBattery(type) = (type == "18500") ? Z_BATTERY_18500 : Z_BATTERY_18650;

module battery(outer, type) {
    dz = zBattery(type);
    color("yellow") translate([0, outer/2 - D_BATTERY/2, 0]) {
        cylinder(d=D_BATTERY, h = dz + EPS2);
    }
}

function dynamicHeatSinkThread() = 20.2;  // 20.4 is loose (PHA), 20.1 tight (PLA)

module dynamicHeatSinkHolder(diameter)
{
    DZ = 10;
    D_HEAT_SINK_THREAD = dynamicHeatSinkThread();

    difference() {
        cylinder(h=DZ, d=diameter);
        translate([0, 0, -EPS]) cylinder(h=100, d=D_HEAT_SINK_THREAD);
    }
}

module shortDynamicHeatSinkHolder(diameter)
{
    DZ = 4.0;
    D_HEAT_SINK_THREAD = dynamicHeatSinkThread();

    difference() {
        cylinder(h=DZ, d=diameter);
        cylinder(h=DZ + EPS, d=D_HEAT_SINK_THREAD);
    }
}


DOTSTAR_XZ = 5.6;
DOTSTAR_PITCH = 7;
DOTSTAR_STRIP_XZ = 12.4;

module dotstarLED(n, dy, pitch=DOTSTAR_PITCH)
{
    for(i=[0:n-1]) {
        translate([-DOTSTAR_XZ/2, 0, pitch * i - DOTSTAR_XZ/2]) {
            cube(size=[DOTSTAR_XZ, dy, DOTSTAR_XZ]);
        }
    }
}

function dotstarStripZLen(n, pitch=DOTSTAR_PITCH) = DOTSTAR_STRIP_XZ + pitch * (n-1);

module dotstarStrip(n, y0=0, y1=1, pitch=DOTSTAR_PITCH)
{
    translate([-DOTSTAR_STRIP_XZ/2, y0, -DOTSTAR_STRIP_XZ/2]) {
        cube(size=[ DOTSTAR_STRIP_XZ, 
                    y1 - y0,
                    dotstarStripZLen(n, pitch)]);
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


module switchCounter()
{
    color("yellow") {
        rotate([90, 0, 0]) {
            cylinder(h=30, d=12);
        }
    }
}


module mc(widePart=0)
{
    color("aqua") {
        translate([-X_MC/2, 0, 0])
            cube(size=[X_MC, Y_MC, Z_MC]);
    
        Y_OFFSET = 4;
        if (widePart > 0) {
            translate([-widePart/2, Y_OFFSET, 0])
                cube(size=[widePart, Y_MC - Y_OFFSET, Z_MC]);
        }
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

module portCounter()
{
    H = 30;
    rotate([90, 0, 0]) {
        cylinder(h=H, d=12);
    }
}

module speakerStd28(bass)
{
    H_SPKR        =  3.2;
    D_SPKR        =  28;
    X_NOTCH       =  6;
    Z_NOTCH       =  6.0;

    color("yellow") {
        cylinder(h=H_SPKR, d=D_SPKR);
        translate([-X_NOTCH/2, 0, 0])
            cube(size=[X_NOTCH, D_SPKR * 0.6, Z_NOTCH]);
    }
}

module speakerStd23()
{
    H_SPKR        =  3.2;
    D_SPKR        =  23;
    X_NOTCH       =  6;
    Z_NOTCH       =  11.5;

    color("yellow") {
        cylinder(h=H_SPKR, d=D_SPKR);
        *translate([-X_NOTCH/2, 0, 0])
            cube(size=[X_NOTCH, D_SPKR * 0.6, Z_NOTCH]);
    }
}


module speakerCLS28(extendY)
{
    color("yellow") {
        cylinder(h=3, d=28);
        cylinder(h=4.5, d=26);

        if (extendY) {
            hull() {
                cylinder(h=7, d=16 + 1.0);
                translate([0, -50, 0]) cylinder(h=7, d=16 + 1.0);
            }
            ACCESS = 6;
            translate([-ACCESS/2, 0, 0]) cube(size=[ACCESS, 20, 4]);
        }
        else {
            cylinder(h=7, d=16 + 1.0);
        }
    }
}

// Intermediate parts -----------------

module baffleHalfBridge(dz, t)
{
    polygonYZ(t, [[0,0], [dz*2, dz*2], [dz*2, 0]]);
}

X_BRIDGE = X_MC/2+1;
T_BRIDGE = 1.6;

module oneBaffleBottonRail(d, dz) 
{
    yMC = -yAtX(X_MC/2, d/2) + 0.5;

    Y = 12.0;
    
    intersection() {
        translate([0, 0, -EPS]) cylinder(h=dz*2 + EPS2, d=d);
        union() {
            translate([TROUGH_2 / 2, yMC - 10, -EPS])
                cube(size=[20, Y, dz*2 + EPS]);
            translate([X_MC / 2, yMC - 10, -EPS])
                cube(size=[20, Y, dz*2 + EPS]);
        }
    }    
}

module bridgeAndRail(bridge, d, dz, bottomRail=true, bridgeOnlyBottom=false)
{

    if (bridge > 0) {
        if (bridge == 1) 
        {
            yBridge = yAtX(X_BRIDGE+T_BRIDGE, d/2);

            translate([X_BRIDGE, -yBridge, dz]) 
                baffleHalfBridge(dz, T_BRIDGE);        
            mirror([1, 0, 0]) translate([X_BRIDGE, -yBridge, dz]) 
                baffleHalfBridge(dz, T_BRIDGE);
            if (!bridgeOnlyBottom) {
                translate([X_BRIDGE, yBridge - dz*2, dz]) 
                    baffleHalfBridge(dz, T_BRIDGE);        
                mirror([1, 0, 0]) translate([X_BRIDGE, yBridge - dz*2, dz]) 
                    baffleHalfBridge(dz, T_BRIDGE);
            }
        }
        else {
            translate([0, 0, dz]) {
                if (bridge == 2)
                    bridge2(d, dz);
                else if (bridge == 3)
                    bridge3(d, dz);
                else if (bridge == 4)
                    bridge4(d, dz);
                else if (bridge == 5)
                    bridge5(d, dz);
            }
        }
        if (bottomRail) {
            oneBaffleBottonRail(d, dz);
            mirror([1,0,0]) oneBaffleBottonRail(d, dz);
        }
    }
}

TROUGH_0 = 12;  // top
TROUGH_1 = 10;  // between battery & mc
TROUGH_2 = 13;  // bottom opening

module oneBaffle(   d,
                    dz,
                    battery=true,       // space for the battery
                    mc=true,            // space for the microcontroller
                    bridge=1,           // create a bridge to the next baffle. designed to print w/o support material. 
                    bridgeOnlyBottom=false, // constrain the type1 bridge to the bottom part
                    dExtra=0,           // additional diameter
                    scallop=false,      // outside curves
                    noBottom=true,      // bottom cutout 
                    cutoutHigh=true,    // open space to the top
                    bottomRail=true,    // bridge must be >0 for a bottom rail
                    conduit=false,
                    slopeFront=false,
                    mcFullDrop=false,
                    batteryOffset=0
                    )     
{
    yMC = -yAtX(X_MC/2, d/2) + 1.0;

    difference() {
        union() {
            if (slopeFront) {
                OVERLAP = 0.8;
                REDUCE = 1;

                cylinder(h=dz - OVERLAP, d=d + dExtra);
                translate([0, 0, dz-OVERLAP])
                    cylinder(h=OVERLAP, d1=d + dExtra, 
                            d2=d + dExtra - 1);
            }
            else {
                cylinder(h=dz, d=d + dExtra);
            }
            E = 0.02;
            bridgeAndRail(bridge, d, dz + E*2, bottomRail=bottomRail, bridgeOnlyBottom=bridgeOnlyBottom);
        }

        if (battery) {
            translate([0, -batteryOffset, -EPS]) battery(d);
        }
        translate([-TROUGH_1/2, -5, -EPS]) 
            cube(size=[TROUGH_1, 5, dz + EPS2]);

        if (mc) {
            translate([0, yMC, -EPS]) 
                mc();
        }
        if (mcFullDrop) {
            translate([-X_MC/2, yMC-20, -EPS])
                cube(size=[X_MC, 20, dz+EPS2]);
        }

        if (scallop) {
            TUNE_X = 1.2;
            TUNE_D = 1.6;

            translate([d*TUNE_X, 0, -EPS]) 
                cylinder(h=dz + EPS2, d=d * TUNE_D);
            mirror([1,0,0])
                translate([d*TUNE_X, 0, -EPS]) 
                    cylinder(h=dz + EPS2, d=d * TUNE_D);
        }

        if (cutoutHigh) {
            translate([-TROUGH_0/2, 0, -EPS]) 
                cube(size=[TROUGH_0, 30, dz + EPS2]);
        }

        if (noBottom) {
            translate([-TROUGH_2/2, -20, -EPS])
                cube(size=[TROUGH_2, 15, dz + EPS2]);
        }

        if (conduit) {
            // Easy wiring, stops battery
            translate([0, 0, -EPS]) {
                hull() {
                    //translate([0, d*0.00, 0]) 
                    //    cylinder(h=dz+EPS2, d = d * 0.6);
                    translate([0, -d*0.1, 0]) 
                        cylinder(h=dz+EPS2, d = d * 0.6);
                }
                translate([-TROUGH_0/2, 0]) 
                    cube(size=[TROUGH_0, 30, dz + EPS2]);
            }
        }
        
        if (isSmall(d)) {
            SMALL_X = 19.0;
            // MC held in by case; requires insulation.
            // Assumes everything is packed in tight. :(
            // Clear out the bottom:
            translate([-SMALL_X/2, -20, -EPS])
                cube(size=[SMALL_X, 20, dz*2 + EPS2]);

            // Trim the bottom.
            // translate([-50, -d/2, 2]) cube(size=[100, 2, dz*2 + EPS2]);

            // And extra opening on top:
            W = 13.0;
            translate([-W/2, 0, -EPS])
                cube(size=[W, 20, dz*2 + EPS2]);
        }
    }
}


// Resuable parts -----------------------------------
/*
    type = 
        "std28"
        "cls28"
        "std23"
 */
module speakerHolder(outer, dz, dzToSpkrBack, type, extraZ=0)
{
    difference() {
        union() {
            if (type == "std28") {
                difference() {
                    tube(h=dz + extraZ, do=outer, di=24);
                    translate([0, 0, dzToSpkrBack])
                        speakerStd28(false);
                }
            }
            else if (type=="cls28") {
                difference() {
                    tube(h=dz + extraZ, do=outer, di=24);
                    translate([0, 0, dzToSpkrBack])
                        speakerCLS28(extendY=true);
                }
            }
            else if (type=="std23") {
                translate([0, 0, dz + extraZ - 4])
                    tube(h=4, do=outer, di=19);
                difference() {
                    tube(h=dz + extraZ, do=outer, di=21);
                    translate([0, 0, dzToSpkrBack])
                        speakerStd23();
                }
            }
            else
                echo("Error speaker type not specified.");
        }
        translate([-50, -outer, -EPS])
            cube(size=[100, outer - 6, 100]);    

        THETA = (type == "std23") ? 10 : 14;
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


module powerPortRing(diameter, t, dz, dzToPort, portSupportToBack=false, counter=true, yTop=0, dyPort=6.0, topNut=true, powerNutCanTurn=false)
{    
    SECTION_PAD = 0.5;
    if (dzToPort - D_NUT/2 - 0.5 < 0) {
        echo("ERROR Port center z too low.");
    }
    if (dzToPort + D_NUT/2 + 0.5 > dz) {
        echo("ERROR Port dz too short");
    }

    D_NUT = 11.6;
    W_NUT = 10.15;
    H_NUT =  2.15;
    H_NUT_PRACT = H_NUT + (diameter/2 - yAtX(W_NUT/2, diameter/2));
    H_THREAD = 5.5;
    D_THREAD = 7.8;
    H_THREAD_PRACT = H_THREAD + (diameter/2 - yAtX(D_THREAD/2, diameter/2));
    yTop = (yTop == 0) ? diameter / 2 : yTop;

    difference() {
        union() {
            tube(h=dz, do=diameter, di=diameter-t);
            intersection() {
                cylinder(h=dz, d=diameter);

                portY = topNut ? (yTop - H_THREAD_PRACT) 
                                : yTop - dyPort;

                if (portSupportToBack) {
                    translate([-50, portY, 0])
                        cube(size=[100, 50, D_PORT_SUPPORT/2 + dzToPort]);
                }
                else {
                    translate([-50, portY, dzToPort - D_PORT_SUPPORT/2])
                        cube(size=[100, 50, D_PORT_SUPPORT]);
                }
            }
        }
        translate([0, 0, dzToPort]) {
            rotate([-90, 0, 0]) {
                D = topNut ? D_PORT + 0.1 : D_PORT;
                cylinder(h=100, d=D);
            }
            if (counter) 
                portCounter();
        }
        if (topNut) {
            translate([0, yTop - H_NUT_PRACT, dzToPort]) {
                if (powerNutCanTurn) {
                    rotate([-90, 0, 0])
                        cylinder(h=100, d=D_NUT);
                }
                else {
                    intersection() {
                        rotate([-90, 0, 0])
                            cylinder(h=100, d=D_NUT);
                        translate([-W_NUT/2, 0, -50])
                            cube(size=[W_NUT, 100, 100]);
                    }
                }
            }
        }
    }
}


module boltRingNoShell(dy_port, bolt_d=0, nut_w=0, nut_y=0)
{
    BOLT_D = bolt_d > 0 ? bolt_d : 4.5;
    NUT_W  = nut_w > 0  ? nut_w  : 8.6;
    NUT_Y  = nut_y > 0  ? nut_y  : 3.4;
    DZ_BRIDGE = 8.0;
    DZ_SLOT = 2.0;
    
    H_THREAD = 5.5;
    D_THREAD = 7.8;

    TRI = [[-50, 0], [0, 0], [-50, -50]];

    difference() {
        union() {
            // Top of the holder aligns witht the port, so the nut can be slid in.
            translate([-50, dy_port, -DZ_BRIDGE/2]) cube(size=[100, 100, DZ_BRIDGE]);                    
        }

        rotate([-90, 0, 0]) cylinder(h=100, d=BOLT_D);
        translate([-NUT_W/2, 0, DZ_SLOT/2]) cube(size=[NUT_W, 100, DZ_SLOT]);
    }
    // Side blocks
    {
        translate([-50, dy_port - NUT_Y, -DZ_BRIDGE/2]) 
            cube(size=[50 - NUT_W/2, NUT_Y, DZ_BRIDGE]);
        translate([-NUT_W/2, dy_port - NUT_Y, -DZ_BRIDGE/2]) 
            polygonXY(h=DZ_BRIDGE, points=TRI);
    }
    mirror([-1, 0, 0]) {
        translate([-50, dy_port - NUT_Y,  -DZ_BRIDGE/2]) 
            cube(size=[50 - NUT_W/2, NUT_Y, DZ_BRIDGE]);
        translate([-NUT_W/2, dy_port - NUT_Y, -DZ_BRIDGE/2]) 
            polygonXY(h=DZ_BRIDGE, points=TRI);
    }
    
    // Triangle holders
    translate([-BOLT_D/2, dy_port - NUT_Y, -DZ_SLOT/2]) {
        polygonXY(h=DZ_SLOT, points=TRI);
    }
    mirror([-1, 0, 0]) translate([-BOLT_D/2, dy_port - NUT_Y, -DZ_SLOT/2]) {
        polygonXY(h=DZ_SLOT, points=TRI);
    }
}


module boltRing(diameter, t, dz, dzToBolt, bolt_d=0, nut_w=0, nut_y=0, dy_port=0)
{
    BOLT_D = bolt_d > 0 ? bolt_d : 4.5;
    NUT_W  = nut_w > 0  ? nut_w  : 8.6;
    NUT_Y  = nut_y > 0  ? nut_y  : 3.4;
    DZ_BRIDGE = 8.0;
    DZ_SLOT = 2.0;
    
    H_THREAD = 5.5;
    D_THREAD = 7.8;
    DY_PORT = dy_port > 0 ? dy_port :
        diameter/2 - (H_THREAD + (diameter/2 - yAtX(D_THREAD/2, diameter/2)));    // from portRing

    TRI = [[-50, 0], [0, 0], [-50, -50]];

    if (dz < DZ_BRIDGE) {
        echo("ERROR: boltRing dz too small. Should be:", DZ_BRIDGE);
    }

    intersection() {
        cylinder(h=dz, d=diameter);
        union() {
            difference() {
                union() {
                    tube(h=dz, do=diameter, di=diameter - t);
                    // Top of the holder aligns witht the port, so the nut can be slid in.
                    translate([-50, DY_PORT, dzToBolt - DZ_BRIDGE/2]) cube(size=[100, 100, DZ_BRIDGE]);                    
                }

                translate([0, 0, dzToBolt]) rotate([-90, 0, 0]) cylinder(h=100, d=BOLT_D);
                translate([-NUT_W/2, 0, dzToBolt - DZ_SLOT/2]) cube(size=[NUT_W, 100, DZ_SLOT]);
            }
            // Side blocks
            {
                translate([-50, DY_PORT - NUT_Y, dzToBolt - DZ_BRIDGE/2]) 
                    cube(size=[50 - NUT_W/2, NUT_Y, DZ_BRIDGE]);
                translate([-NUT_W/2, DY_PORT - NUT_Y, dzToBolt - DZ_BRIDGE/2]) 
                    polygonXY(h=DZ_BRIDGE, points=TRI);
            }
            mirror([-1, 0, 0]) {
                translate([-50, DY_PORT - NUT_Y, dzToBolt - DZ_BRIDGE/2]) 
                    cube(size=[50 - NUT_W/2, NUT_Y, DZ_BRIDGE]);
                translate([-NUT_W/2, DY_PORT - NUT_Y, dzToBolt - DZ_BRIDGE/2]) 
                    polygonXY(h=DZ_BRIDGE, points=TRI);
            }
            
            // Triangle holders
            translate([-BOLT_D/2, DY_PORT - NUT_Y, dzToBolt - DZ_SLOT/2]) {
                polygonXY(h=DZ_SLOT, points=TRI);
            }
            mirror([-1, 0, 0]) translate([-BOLT_D/2, DY_PORT - NUT_Y, dzToBolt - DZ_SLOT/2]) {
                polygonXY(h=DZ_SLOT, points=TRI);
            }

        }
    }
}


module switchRing(diameter, t, dz, dzToSwitch, counter=true, switchDY=0)
{
    difference() {
        union() {
            tube(h=dz, do=diameter, di=diameter-t);
            intersection() {
                cylinder(h=dz, d=diameter);
                translate([-50, diameter/2 - 7, dzToSwitch - D_SWITCH_SUPPORT/2]) {
                    cube(size=[100, 50, D_SWITCH_SUPPORT]);
                }
            }
        }
        translate([0, switchDY, dzToSwitch]) {
            switch(diameter, true);
            if (counter) switchCounter();
        }
    }
}


// Pin ignition keeps getting re-designed.
// Current design: upside down 8-32 1/4" socket head screw.
// Pin
module tactileRing(diameter, t, dz, dzToTactile, hasPinIgnition, dyToPlateBase=0)
{
    TACTILE_X = 6.2;
    TACTILE_PAD = 1.0;   
    D_PIN_BASE = 10.5 + 0.4;
    TACTILE_BASE_TO_PLATE = 8.0 + 0.5;  // assuming 8mm switch, which is nicely in the middle

    DY = hasPinIgnition ? (dyToPlateBase - TACTILE_BASE_TO_PLATE) : 2.0;
    W = 11.0;

    if(dzToTactile - TACTILE_X/2 - TACTILE_PAD < 0)
        echo("ERROR too little space behind tactile switch (for switch)");
    if(dzToTactile + TACTILE_X/2 + TACTILE_PAD > dz)
        echo("ERROR too little space in front of tactile switch (for switch)");
    if (!hasPinIgnition) {
        if(dzToTactile - D_PIN_BASE/2 < 0)
            echo("ERROR too little space behind tactile switch (for pin base)");
        if(dzToTactile + D_PIN_BASE/2 > dz)
            echo("ERROR too little space in front tactile switch (for pin base)");
    }

    difference() {
        intersection() 
        {
            cylinder(h=100, d=diameter);
            union() {
                difference() {
                    tube(h=dz, do=diameter, di=diameter - t);
                    translate([-W/2, 0, 0]) cube(size=[W, 100, 100]);
                }
                translate([0, 0, dzToTactile]) {
                    simpleBridge(diameter, DY, 1.0, TACTILE_X, addWidth=TACTILE_X, flatFill=false);
                }
                translate([0, 0, dzToTactile - TACTILE_X/2]) {
                    W = 16.0;
                    difference() {
                        translate([-W/2, DY, 0]) cube(size=[W, 4.0, TACTILE_X]);
                        translate([-TACTILE_X/2, DY, 0]) cube(size=[TACTILE_X, 20, TACTILE_X + EPS]);
                    }
                }
            }
        }
        H = 6;
        Z0 = dzToTactile - TACTILE_X/2;
        Z1 = dz - (dzToTactile + TACTILE_X/2);
        if (Z0 > 2.0 && Z1 > 1.0) {
            hull() {            
                translate([-50, -H + Z0/2, Z0/2]) rotate([0, 90, 0]) cylinder(h=100, d=Z0);
                translate([-50, H - Z0/2, Z0/2]) rotate([0, 90, 0]) cylinder(h=100, d=Z0);
            }
            hull() {            
                translate([-50, -H + Z1/2, dz - Z1/2]) rotate([0, 90, 0]) cylinder(h=100, d=Z1);
                translate([-50, 0, dz - Z1/2]) rotate([0, 90, 0]) cylinder(h=100, d=Z1);
            }
        }
    }
}

module emitterCouplerRing(diameter, t, dz)
{
    HEADER_H = 8.4;
    HEADER_W = 2.54;
    HEADER_Y = 2.54 * 4;
    PCB_Z = 1.8;
    OVERLAP = 1.0;
    PCB_D = 20.0;

    PILLAR_W = 8.0;
    RING_CUT = 8.0;
    PCB_BACK = dz - HEADER_H - PCB_Z;

    //if (COLUMN_BACK > dz) echo("ERROR emitterCouplerRing dz too small");

    difference() {
        tube(h=dz, do=diameter, di=19);
        // top cutout
        translate([-50, 4, 0]) //PCB_BACK]) 
            cube(size=[100, 50, 100]);
        // ring cutout
        //translate([-RING_CUT/2, diameter/2 - 4, -10]) 
        //    cube(size=[RING_CUT, 50, 100]);
        // pcb slot
        hull() {
            translate([0, 0, PCB_BACK]) cylinder(h=PCB_Z, d=PCB_D);
            translate([0, 20.0, PCB_BACK]) cylinder(h=PCB_Z * 0.5, d=PCB_D);
        }
    }

    intersection() {
        translate([0, 0, -10]) cylinder(h=100, d=diameter);

        union() {

            // front holder
            SLOPE = 1.5;
            difference() {
                translate([-PILLAR_W/2, -diameter/2, dz - HEADER_H])
                    cube(size=[PILLAR_W, diameter/2 + HEADER_H/2, HEADER_H + SLOPE]);
                translate([-HEADER_W/2, -HEADER_Y/2 - 2, dz - HEADER_H])
                    cube(size=[HEADER_W, 100, 100]);
                hull() {
                    translate([-HEADER_W/2, -diameter/2, dz]) 
                        cube(size=[HEADER_W, 100, SLOPE]);
                    translate([-PILLAR_W/2, -diameter/2, dz + SLOPE]) 
                        cube(size=[PILLAR_W, 100, 1.0]);
                }
            }
        }
    }

}

module baffleMCBattery2(d, nBaffles, dz) 
{
    slices = nBaffles * 2 - 1;
    dzSlice = dz / slices;

    LONG = 200;
    D = 50;

    OFF0 = 1;
    OFF1 = 18;
    MC_BASE =  d/2 - D_BATTERY - Y_MC;

    difference() {
        cylinder(h=dz, d=d);

        hull() {
            translate([0, d/2 - D_BATTERY/2, -EPS])
                cylinder(h=LONG, d=D_BATTERY);
            translate([0, 50, -EPS])
                cylinder(h=LONG, d=D_BATTERY);
        }
        translate([-X_MC/2, MC_BASE, -EPS])
            cube(size=[X_MC, Y_MC, LONG]);

        // Top
        translate([-TROUGH_0/2, 5, -EPS]) cube(size=[TROUGH_0, 20, LONG]);
        // Middle
        translate([-TROUGH_1/2, -5, -EPS]) cube(size=[TROUGH_1, 10, LONG]);
        // Bottom
        translate([-TROUGH_2/2, -d/2-1, -EPS]) cube(size=[TROUGH_2, 10, LONG]);

        // Cut the baffles:
        for(i=[0:nBaffles-1]) {
            translate([0, 0, dzSlice + i * 2 * dzSlice]) {
                tube(h=dzSlice, di=d - OFF0, do=D);  // outside
                cylinder(h=dzSlice, d=d - OFF1);
            }
        }

        // Don't want overhang on the cuts.
        // Only matters at larger D
        for(i=[0:nBaffles-1]) {
            translate([-X_MC/2, MC_BASE, dzSlice + i * 2 * dzSlice]) {
                cube(size=[X_MC, D, dzSlice]);
            }
        }


        // punch the vents:
        for(i=[0:nBaffles-1]) {
            slope = dzSlice;
            H = 4;
            translate([-D/2, d*0.00, dzSlice + i * 2 * dzSlice]) {
                polygonYZ(h=D, points=[
                    [0, 0], [slope, dzSlice],
                    [H+slope, dzSlice], [H, 0]
                ]);
            }
            translate([-D/2, -d*0.32, dzSlice + i * 2 * dzSlice]) {
                polygonYZ(h=D, points=[
                    [0, 0], [slope, dzSlice],
                    [H+slope, dzSlice], [H, 0]
                ]);
            }
            // Thermal relief. Not sure it's needed for new printer.
            if ((i-2)%3 == 0) {
                T = X_MC;
                translate([-T/2, -d/2, dzSlice + i * 2 * dzSlice]) {
                    cube(size=[T, 5, dzSlice]);
                }
            }
        }

    }
}


module pcbButtress(buff=0)
{    
    BUTTRESS_T = 5 + buff;
    difference() {
        translate([0,0,-BUTTRESS_T/2])
            polygonXY(BUTTRESS_T, 
                [[-3,0], [-2,-2], [14,-30], [14,0]]
            );
        translate([0, -50, 0]) rotate([-90, 0, 0]) cylinder(h=50, d=D_M2);
        translate([0, -1.0, 0]) rotate([-90, 0, 0])
            cylinder(h=1.1, d2=D_M2 + 0.5, d1=D_M2);
    } 
}

module pcbPillar(dBoost=0) {
    difference() {
        translate([0, -50, 0]) 
            rotate([-90, 0, 0])
                difference() {
                    cylinder(h=50, d1=8 + dBoost, d2=5 + dBoost);
                    cylinder(h=100, d=D_M2 + 0.4);
                }
        translate([0, -1, 0]) rotate([-90, 0, 0])
            cylinder(h=1.1, d2=D_M2 + 0.5, d1=D_M2);
    }
}

module pcbHolder2(d, t, dzSection, dzToPCB, pcbDim, mount) 
{
    W = pcbDim[0];
    Y = pcbDim[1];
    DZ = pcbDim[2];

    difference() {
        tube(h=dzSection, do=d, di=d-t);
        translate([-W/2, 0, dzToPCB]) {
            cube(size=[W, d, DZ]);
        }
    }
    intersection() {
        cylinder(h=dzSection, d=d);
        for(m = mount) {
            translate([0, Y, dzToPCB]) {
                color("plum") union() {
                    x = m[0];
                    z = m[1];
                    type = m[2];
                    translate([x, 0, dzToPCB + z]) 
                    {
                        if (type == "pillar") {
                            pcbPillar();
                        }
                        else if (type == "buttress") {
                            shouldMirror = x < 0;
                            if (shouldMirror)
                                mirror([1,0,0]) pcbButtress();
                            else
                                pcbButtress();
                        }
                    }            
                }
            }
        }
    }
}

/*
    Render the emitter holder for a advanced LED heatsink.
    Should render at the same place as the base:
    translate([0, 0, z]) {
        emitterBase(d, dz);
        emitterHolder(d, dz);
    }
*/

H_TEETH             = 4.0;
H_ADVANCED_THREAD   = 13.8; // larger to make threading easier
H_RING              = 3.0;
H_HEATSINK          = 0.5 + 0.890 * INCHES_TO_MM;
D_ADVANCED_LED      = 20.000; // hole where the light shines.
D_HEATSINK          = 25.6;


module emitterTeeth(d, delta=0, isCutout=false)
{
    N_ADVANCED_TEETH = 6;
    TEETH_Y = 7 + delta;

   intersection() {
        tube(h=H_TEETH, do=isCutout ? 100 : d, di=isCutout ? 1.0 : D_HEATSINK);
        union() {
            for(r=[0:5]) {
                rotate([0, 0, r*60])
                    translate([0, -TEETH_Y/2, 0])
                        cube(size=[50, TEETH_Y, H_TEETH]);
            }
        }
   }
}

function emitterZ() = H_ADVANCED_THREAD + H_HEATSINK + H_RING;

module emitterHolder(d)
{
    translate([0, 0, H_ADVANCED_THREAD]) 
    difference() {
        union() {
            // Top cap
            translate([0, 0, H_HEATSINK])
                cylinder(h=H_RING, d=d);        
            // LED
            cylinder(h=H_HEATSINK, d=d);
        }
        BEPS = 1;   // offset back to clean up noise
        translate([0, 0, -BEPS])
        {
        // Center light
            cylinder(h=100, d=D_ADVANCED_LED);
            // Heatsink
            cylinder(h=H_HEATSINK + BEPS, d=D_HEATSINK);
        }
        // Vents / decoration / volume reduction
        Z0 = 4;
        Z1 = 12;
        Z2 = 21;

        for(r=[0:5]) {
            rotate([0, 0, r*60])
                polygonXZ(h=50, points=
                [
                    [-5, Z2],
                    [5, Z2],
                    [5, Z1],
                    [0, Z0],
                    [-5, Z1]
                ]);
        }
    }
    translate([0, 0, H_ADVANCED_THREAD - H_TEETH]) {
        emitterTeeth(d);
    }
}

module emitterBase(d)
{
    //color("olive") 
    {
        difference() {
            tube(h=H_ADVANCED_THREAD-EPS, do=d, di=dynamicHeatSinkThread());
            translate([0, 0, H_ADVANCED_THREAD - H_TEETH]) emitterTeeth(0.1);
        }
    }
}

/*
    Render the front (1", typically) mount for the advanced LED,
    port, and switch. Designed to be printed z-axis up, with
    minimal support.

    Note on advanced heat sink:
    Len (housing) = 22.8mm
    Len (thread) = 10.0mm
    
*/
module forwardAdvanced(d, dz, 
    overlap,    // if the length (backward) of the overlap area
    outer,      // diameter of overlap ring
    dzToPort, dzToSwitch)
{
    LED_DZ = 10.0;    
    D_INNER = dynamicHeatSinkThread();
    DIAMETER = d;
    T = 4;

    difference() {
        union() {              
            // Front heatsink holder.
            translate([0, 0, dz - LED_DZ]) 
                dynamicHeatSinkHolder(DIAMETER);

            // The front body
            translate([0, 0, 0]) {
                tube(h=dz, do=DIAMETER, di=D_INNER);
            }

            // Thicken top part.
            {
                MOUNT = 5.5;
                
                difference() 
                {
                    intersection() {
                        cylinder(h=dz, d=DIAMETER);
                        translate([-20, DIAMETER/2 - MOUNT, 0])
                            cube(size=[40, MOUNT, dz - LED_DZ]);
                    }
                }
            }

            // overlap ring
            if (overlap) {
                translate([0, 0, -overlap]) {
                    difference() {
                        cylinder(h=overlap, d=outer);
                        //cylinder(h=overlap, d1=DIAMETER, d2=D_INNER);
                    }
                }
            }
        }
        
        //D = DIAMETER - D_INNER;
        translate([0, 0, -overlap]) cylinder(h=DIAMETER, d1=DIAMETER, d2=0);

        // Side access.
        {
            H = 14;
            OFFSET = 2;
            translate([0, 0, 0]) {
                translate([0, -H/2, OFFSET]) {
                    cube(size=[50, H, dz - LED_DZ - OFFSET]);
                    mirror([1,0,0]) cube(size=[50, H, dz - LED_DZ - OFFSET]);
                }
            }
        }

        if (dzToPort) {
            translate([0, 0, dzToPort]) {
                port(true);
                portCounter();
            }
        }

        if (dzToSwitch) {
            translate([0, 0, dzToSwitch]) {
                switch(DIAMETER, true);
                switchCounter();
            }
        }
    }
}


$fn = 40;
*oneBaffle(30, 4, 
    battery=false,
    mc=false,
    bridge=1,
    noBottom=false,
    cutoutHigh=false,
    conduit=true);

*keyJoint(8, 30, 26, false);

*boltRing(30.0, 4.0, 12.0, 6.0);

*baffleMCBattery( 32,    // outer diameter 
                  12,    // number of baffles 
                  Z_BATTERY_18650);

*baffleMCBattery2( 32,    // outer diameter 
                  12,    // number of baffles 
                  Z_BATTERY_18650);
*color("red") battery(32, "18650");
