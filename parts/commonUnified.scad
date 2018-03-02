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

EPS = 0.01;
EPS2 = EPS * 2;

// Math ------------
function yAtX(x, r) = sqrt(r*r - x*x);

// Utilities, shapes. ------------------------
module capsule(theta0, theta1, r=2)
{
    hull() {
        rotate([-90, -0, theta0]) cylinder(h=20, r=r);
        rotate([-90, -0, theta1]) cylinder(h=20, r=r);
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

module crystal()
{
    TAPER = 5;
    translate([0, DY_CRYSTAL, 0]) {
        color("pink") scale([1.0, Y_CRYSTAL/X_CRYSTAL, 1.0]) {
            cylinder(d=X_CRYSTAL, h=Z_CRYSTAL - TAPER);
            translate([0, 0, Z_CRYSTAL - TAPER])
                cylinder(h=TAPER, d1 = X_CRYSTAL, d2=X_CRYSTAL *0.7);
        }
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

module baffledBatMC(d, dzButtress, dFirst, dzFirst)
{
    N = ceil(Z_PADDED_BATTERY / (dzButtress*2));

    for(i=[0:N-1]) {
        translate([0, 0, i*dzButtress*2]) 
            if (i==0)
                oneBaffle(d, dzFirst, dExtra=dFirst - d);
            else
                oneBaffle(d, dzButtress);
    }
}