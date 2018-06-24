use <shapes.scad>

// TODO: unify the OledHolder and the PCB holder

Z_BATTERY           =  68;
Z_PADDED_BATTERY    =  Z_BATTERY + 1;
D_BATTERY           =  18.50 + 0.5;    // An 1850. Huh. 

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

module zCapsule(dz, r)
{
    hull() {
        rotate([-90, 0, 0]) cylinder(h=20, r=r);
        translate([0, 0, dz]) rotate([-90, 0, 0]) cylinder(h=20, r=r);
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
    secure. Problematic is that it click together on the y
    axis so it's hard to run the wires.
*/
module cylinderKeyJoint(dz, do, di, trim)
{
    RATIO = 0.15; 

    intersection() {
        tube(h=dz - trim, do=do, di=di);
        translate([-do, -do*RATIO, 0])
            cube(size=[do*2.0, do*RATIO*2, dz]);
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
module battery(outer) {
    color("yellow") translate([0, outer/2 - D_BATTERY/2, 0]) {
        cylinder(d=D_BATTERY, h = Z_BATTERY + EPS2);
    }
}


DOTSTAR_XZ = 5.6;
DOTSTAR_PITCH = 7;
// FIXME: correct values for strip size.
DOTSTAR_STRIP_XZ = 8;

module dotstarLED(n, dy)
{
    for(i=[0:n-1]) {
        translate([-DOTSTAR_XZ/2, 0, DOTSTAR_PITCH*i - DOTSTAR_XZ/2]) {
            cube(size=[DOTSTAR_XZ, dy, DOTSTAR_XZ]);
        }
    }
}


module dotstarStrip(n, y0=0, y1=1)
{
    translate([-DOTSTAR_STRIP_XZ/2, y0, -DOTSTAR_STRIP_XZ/2]) {
        cube(size=[ DOTSTAR_STRIP_XZ, 
                    y1 - y0,
                    DOTSTAR_STRIP_XZ + DOTSTAR_PITCH * (n-1)]);
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
                    dExtra=0,
                    scallop=false,
                    cutout=true)
{
    yMC = -yAtX(X_MC/2, d/2) + 1.0;

    difference() {
        cylinder(h=dz, d=d + dExtra);
        if (battery)
            battery(d);
        if (mc) {
            translate([0, yMC, -EPS]) 
                mc();
        }
        if (mcSpace)
            translate([-X_MC/2, DY_MC, -EPS])
                cube(size=[X_MC, 20, dz+EPS2]);
        if (scallop) {
            TUNE_X = 1.2;
            TUNE_D = 1.6;

            translate([d*TUNE_X, 0, -EPS]) 
                cylinder(h=dz + EPS2, d=d * TUNE_D);
            mirror([1,0,0])
                translate([d*TUNE_X, 0, -EPS]) 
                    cylinder(h=dz + EPS2, d=d * TUNE_D);
        }

            translate([-TROUGH_0/2, 0, -EPS]) 
                cube(size=[TROUGH_0, 30, dz + EPS2]);
        if (cutout) {
            translate([-TROUGH_1/2, -5, -EPS]) 
                cube(size=[TROUGH_1, 5, dz + EPS2]);
            translate([-TROUGH_2/2, -20, -EPS])
                cube(size=[TROUGH_2, 15, dz + EPS2]);
        }
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
        "bass28"
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

module baffleMCBattery( outer,          // outer diameter 
                        n,              // number of baffles 
                        dzButtress,     // thickness of the baffle
                        dFirst,         // make the back baffle this diameter
                        dzFirst,        // make the back baffle this thicknes 
                        extraBaffle=0)  // add this much to the front baffle
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
    if (extraBaffle) {
        translate([0, 0, (n*2 - 1) * dzButtress]) {
            oneBaffle(outer, extraBaffle, bridge=false);
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

module pcbPillar() {
    translate([0, -50, 0]) 
        rotate([-90, 0, 0])
            difference() {
                cylinder(h=50, d1=8, d2=5);
                cylinder(h=50, d=D_M2);
            }
}

/*
    outer: diameter of the saber. If 0, not clipped.
    t: thickness of the wall
    dz: length of the section
    dzToPCB: delta to where the pcb start
    dyPCB: y delta to PCB bottom.
    size[3]: outer size of the pcb
    mount: array of:
        x location, z location, "pillar" or "buttress"
    makeSection: if true, this is a section of the saber, else
                 just the basic parts are generated.
*/
module pcbHolder(outer, t, dz, dzToPCB, dyPCB, size, mount, makeSection=true)
{
    difference() 
    {
        union() {
            if (makeSection)
               tube(h=dz, do=outer, di=outer-t);

            intersection() {
                cylinder(h=dz, d=outer ? outer : 100);

                color("plum") union() {
                    for(m = mount) {
                        x = m[0];
                        z = m[1];
                        type = m[2];
                        translate([x, dyPCB, dzToPCB + z]) 
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
        translate([-size[0]/2, dyPCB, dzToPCB]) 
            cube(size=[size[0], size[1], size[2]]);
    }
}

OLED_DISPLAY_W           = 23 + 1;
OLED_DISPLAY_L           = 32 + 1;
OLED_DISPLAY_MOUNT_W     = 17;
OLED_DISPLAY_MOUNT_L     = 26;

module oledHolder(outer, t, dz, dzToPCB, dyPCB)
{
    pcbHolder(outer, t, dz, dzToPCB, dyPCB, 
              [OLED_DISPLAY_W, 20, OLED_DISPLAY_L],
              [
                [-OLED_DISPLAY_MOUNT_W/2, (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L)/2, "buttress" ],
                [ OLED_DISPLAY_MOUNT_W/2, (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L)/2, "buttress" ],
                [-OLED_DISPLAY_MOUNT_W/2, OLED_DISPLAY_MOUNT_L + (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L)/2, "buttress" ],
                [ OLED_DISPLAY_MOUNT_W/2, OLED_DISPLAY_MOUNT_L + (OLED_DISPLAY_L - OLED_DISPLAY_MOUNT_L)/2, "buttress" ]
              ]);
}
