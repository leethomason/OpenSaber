use <shapes.scad>
use <baffles.scad>

// TODO: unify the OledHolder and the PCB holder

Z_BATTERY_18650     = 65 + 4;
Z_BATTERY_18500     = 50 + 4;
D_BATTERY           = 18.50 + 0.5;    // An 1850. Huh. 

D_SWITCH            =  12.5;     // actually 12, by thread.
D_SWITCH_SUPPORT    =  16;
DY_SWITCH            =  -2;

D_PORT              =  7.9;
D_PORT_SUPPORT      =  12;
H_PORT              =  16;
DY_PORT             =  6;

// Dimensions for Teensy3.5 with OpenSaber shield
X_MC                =  18.5;
Y_MC                =   9.0;
Z_MC                =  71.0;     // includes SD
DY_MC               = -12.5;

X_CRYSTAL           =  11;
Y_CRYSTAL           =   8.5;
Z_CRYSTAL           =  30.0;

D_M2                = 1.7;

EPS = 0.01;
EPS2 = EPS * 2;

// Math ------------

function yAtX(x, r) = sqrt(r*r - x*x);

// Utilities, shapes. ------------------------
module capsule(theta0, theta1, r=2, _mirror=false)
{
    hull() {
        rotate([-90, -0, theta0]) cylinder(h=20, r=r);
        rotate([-90, -0, theta1]) cylinder(h=20, r=r);
    }
    if (_mirror == true) hull() {
        mirror([1,0,0]) rotate([-90, -0, theta0]) cylinder(h=20, r=r);
        mirror([1,0,0]) rotate([-90, -0, theta1]) cylinder(h=20, r=r);
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

module tjoint(outer, dz, yTrim, zTrim)
{
    RATIO = 0.40; 
    DY = outer * RATIO;
    HALFY = DY * 0.7;

    translate([0, -HALFY/2 - yTrim/2, 0]) 
        cube(size=[100, HALFY + yTrim, dz]);

    translate([0, -DY/2 - yTrim/2, dz/2 - zTrim/2]) 
        cube(size=[100, DY + yTrim, dz/2 + zTrim*2]);
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
    YTRIM = slot ? 0.4 : 0.0;
    ZTRIM = slot ? 0.2 : 0.0;

    intersection() {
        if (slot) {
            tube(h=DZ+2, di=di, do=do+1);
        }
        else {
            difference() {
                union() {
                    cylinder(h=dz-1, d=do);
                    translate([0, 0, dz-1]) cylinder(h=1, d1=do, d2=do-1);
                }
                cylinder(h=100, d=di);
            }
        }

        union() {
            rotate([0, 0, angle]) 
                tjoint(do, dz, YTRIM, ZTRIM);
            rotate([0, 0, 180-angle]) 
                tjoint(do, dz, YTRIM, ZTRIM);
        }
    }
}

module cJoint(dz, dOuter, trim)
{
    XC = dOuter * 0.35;
    XT = dOuter * 0.08;
    D  = dOuter * 0.12;

    translate([XC - XT/2, -50, 0])
        cube(size=[XT, 100, dz]);
    translate([XC, -50, dz])
        rotate([-90, 0, 0])
            cylinder(d=D - trim, h=100);
}

module columnJoint(dz, dOuter, trim)
{
    intersection() {
        cylinder(h=100, d=dOuter);
        union() {
            cJoint(dz, dOuter, trim);
            mirror([-1, 0, 0]) cJoint(dz, dOuter, trim);
        }
    }
}

module trainBridge(x, y, z, nBridge)
{
    dz = z / (nBridge * 2);

    difference() 
    {
        cube(size=[x, y, z]);
        for(i=[0:nBridge-1]) {
            translate([0, 0, i*dz*2]) {
                polygonYZ(h=10, points=[
                    [0, 0], [y*0.5, 0], [y*0.75, dz/2], [y*0.5, dz], [0, dz]
                ]);
            }
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

module speakerStd23(bass)
{
    H_SPKR        =  3.2;
    D_SPKR        =  23;
    X_NOTCH       =  6;
    Z_NOTCH       =  11.5;

    color("yellow") {
        cylinder(h=H_SPKR, d=D_SPKR);
        translate([-X_NOTCH/2, 0, 0])
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
                    conduit=false
                    )     
{
    yMC = -yAtX(X_MC/2, d/2) + 1.0;

    difference() {
        cylinder(h=dz, d=d + dExtra);
        if (battery) {
            translate([0, 0, -EPS]) battery(d);
        }
        translate([-TROUGH_1/2, -5, -EPS]) 
            cube(size=[TROUGH_1, 5, dz + EPS2]);

        if (mc) {
            translate([0, yMC, -EPS]) 
                mc();
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
    }

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
            }
        }
        if (bottomRail) {
            oneBaffleBottonRail(d, dz);
            mirror([1,0,0]) oneBaffleBottonRail(d, dz);
        }
    }
}


// Resuable parts -----------------------------------
/*
    type = 
        "bass22"
        "std28"
        "bass28"
        "cls28"
 */
module speakerHolder(outer, dz, dzToSpkrBack, type, extraZ=0)
{
    difference() {
        union() {
            if(type == "bass22") {
                difference() {
                    tube(h=dz + extraZ, do=outer, di=18);
                    translate([0, 0, dzToSpkrBack])
                        speakerBass22();
                }
            }
            else if (type == "std28" || type == "bass28") {
                difference() {
                    tube(h=dz + extraZ, do=outer, di=24);
                    translate([0, 0, dzToSpkrBack])
                        speakerStd28(type == "bass28");
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
                difference() {
                    tube(h=dz + extraZ, do=outer, di=22);
                    translate([0, 0, dzToSpkrBack])
                        speakerStd23(extendY=true);
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


module powerPortRing(diameter, t, dz, dzToPort, portSupportToBack=false, counter=true)
{    
    difference() {
        union() {
            tube(h=dz, do=diameter, di=diameter-t);
            intersection() {
                cylinder(h=dz, d=diameter);
                if (portSupportToBack) {
                    translate([-50, diameter/2 - DY_PORT, 0])
                        cube(size=[100, 50, D_PORT_SUPPORT/2 + dzToPort]);
                }
                else {
                    translate([-50, diameter/2 - DY_PORT, dzToPort - D_PORT_SUPPORT/2])
                        cube(size=[100, 50, D_PORT_SUPPORT]);
                }
            }
        }
        translate([0, 0, dzToPort]) {
            port(true);
            if (counter) portCounter();
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

function nBafflesNeeded(dzButtress, type) = ceil(zBattery(type) / (dzButtress*2));

function zLenOfBaffles(n, dzButtress) = n * dzButtress * 2 - dzButtress;

module baffleMCBattery( outer,          // outer diameter 
                        n,              // number of baffles 
                        dzButtress,     // thickness of the baffle
                        nPostBaffles=0,// number of additional baffles, that don't have the battery cutout
                        dFirst=0,       // make the back baffle this diameter (0 to use standard)
                        dzFirst=0,      // make the back baffle this thicknes  (0 to use standard)
                        extraBaffle=0,  // add this much to the front baffle
                        bridgeInFront=false,    // set true to contiue bridge. Useful for attaching to a cap.
                        bridgeStyle = 1,
                        mc=true
                    )
{
    totalN = n + nPostBaffles;
    for(i=[0:totalN-1]) {
        translate([0, 0, i*dzButtress*2])  {
            hasBattery = i < n;
            if (i==0 && dFirst > 0 && dzFirst > 0) {
                // First baffle can "overflow" decause of
                // the larger diameter. Use an intersection()
                // to bring it in.
                intersection() {
                    cylinder(h=dzButtress*2, d=dFirst);
                    oneBaffle(outer, dzFirst, 
                            battery=hasBattery,
                            mc=mc,
                            dExtra=dFirst - outer, 
                            bridge=bridgeStyle
                    );
                }
            }
            else {
                oneBaffle(outer, dzButtress, 
                    battery=hasBattery,
                    bridge=bridgeInFront || (i < totalN-1) ? bridgeStyle : 0, mc=mc);
            }
        }
    }
    if (extraBaffle) {
        translate([0, 0, (n*2 - 1) * dzButtress]) {
            oneBaffle(outer, extraBaffle, bridge=0, mc=mc);
        }
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

module pcbPillar(dBoost=0) {
    translate([0, -50, 0]) 
        rotate([-90, 0, 0])
            difference() {
                cylinder(h=50, d1=8 + dBoost, d2=5 + dBoost);
                cylinder(h=100, d=D_M2);
            }
}

/*
    outer: diameter of the saber. If 0, not clipped.
    t: thickness of the wall
    dz: length of the section
    dzToPCB: delta to where the pcb start
    dyPCB: y delta to PCB bottom.
    size[3]: outer size of the pcb.
        x and z are obvious
        y will do a cut above, if > 0
    mount: array of:
        x location, z location, "pillar" or "buttress"
    makeSection: if true, this is a section of the saber, else
                 just the basic parts are generated.
    sizePad: pad the size to make it fit more easily
    angle: rotation of the pcb
*/
module pcbHolder(outer, t, dz, dzToPCB, dyPCB, size, mount, 
    makeSection=true, sizePad=0, holeAccess=false, angle=0)
{
    difference() 
    {
        union() {
            if (makeSection) {
                difference() {
                    tube(h=dz, do=outer, di=outer-t);
                    translate([-size[0]/2 - sizePad, dyPCB, dzToPCB - sizePad]) 
                        cube(size=[size[0] + 2 * sizePad, size[1], size[2] + 2 * sizePad]);
                }
            }

            for(m = mount) {
                intersection() {
                    translate([0, 0, -20]) 
                        cylinder(h=dz+40, d=outer ? outer : 100);

                    translate([0, dyPCB, dz/2])
                    rotate([angle, 0, 0])
                    translate([0, 0, -dz/2])
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
        if (holeAccess == true && angle == 0) {
            for(m = mount) {
                x = m[0];
                z = m[1];
                translate([x, dyPCB, dzToPCB + z]) 
                    rotate([-90, 0, 0])
                        cylinder(h=50, d=4);
            }
        }
    }
}

OLED_DISPLAY_W           = 23 + 1;
OLED_DISPLAY_L           = 32 + 1;
OLED_DISPLAY_MOUNT_W     = 17;
OLED_DISPLAY_MOUNT_L     = 26;

module oledHolder(diameter, t, dz, dzToPCB, dyPCB)
{
    pcbHolder(diameter, t, dz, dzToPCB, dyPCB, 
              [OLED_DISPLAY_W, 20, OLED_DISPLAY_L],
              [
                [-OLED_DISPLAY_MOUNT_W/2, (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L)/2, "buttress" ],
                [ OLED_DISPLAY_MOUNT_W/2, (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L)/2, "buttress" ],
                [-OLED_DISPLAY_MOUNT_W/2, OLED_DISPLAY_MOUNT_L + (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L)/2, "buttress" ],
                [ OLED_DISPLAY_MOUNT_W/2, OLED_DISPLAY_MOUNT_L + (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L)/2, "buttress" ]
              ]);
}

/*
    Render the emitter holder for a advanced LED heatsink.
    Should render at the same place as the base:
    translate([0, 0, z]) {
        emitterBase(d, dz);
        emitterHolder(d, dz);
    }
*/

INCHES_TO_MM        = 25.4;
H_TEETH             = 4.0;
H_ADVANCED_THREAD   = 13.8; // larger to make threading easier
H_RING              = 3.0;
H_HEATSINK          = 0.5 + 0.890 * INCHES_TO_MM;
D_ADVANCED_LED      = 20.000; // hole where the light shines.
D_HEATSINK          = 25.6;


module emitterTeeth(delta=0)
{
    N_ADVANCED_TEETH = 6;
    TEETH_Y = 7 + delta;

    for(r=[0:5]) {
        rotate([0, 0, r*60])
            translate([0, -TEETH_Y/2, 0])
                cube(size=[50, TEETH_Y, H_TEETH]);
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
        intersection() {
            tube(h=H_TEETH, do=d, di=D_HEATSINK);
            emitterTeeth();
        }
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



/*
module oneBaffle(   d,
                    dz,
                    battery=true,       // space for the battery
                    mc=true,            // space for the microcontroller
                    bridge=1,           // create a bridge to the next baffle. designed to print w/o support material. 
                    dExtra=0,           // additional diameter
                    scallop=false,      // outside curves
                    noBottom=true,      // bottom cutout 
                    cutoutHigh=true     // open space to the top

*/

$fn = 80;
oneBaffle(30, 4, 
    battery=false,
    mc=false,
    bridge=1,
    noBottom=false,
    cutoutHigh=false,
    conduit=true);

//translate([0, 0, 1]) battery(30);