use <shapes.scad>

Z_BATTERY           =  68;
D_BATTERY           =  18.50 + 0.5;    // An 1850. Huh. 
DY_BATTERY          =  0;

D_SWITCH            =  12.5;     // actually 12, by thread.
Y_SWITCH            =  13.5;

H_SPKR_METAL        =  4;
D_SPKR_METAL        =  18; 
H_SPKR_PLASTIC      =  4.6;
D_SPKR_PLASTIC      =  20;

D_PORT              =  7.9;
D_PORT_SUPPORT      =  12;
H_PORT              =  16;
DY_PORT             =  5;

EPS = 0.01;
EPS2 = EPS * 2;

// Utilities, shapes. ------------------------
module capsule(theta0, theta1, r=2)
{
    hull() {
        rotate([-90, -0, theta0]) cylinder(h=20, r=r);
        rotate([-90, -0, theta1]) cylinder(h=20, r=r);
    }
}


// Physical components. ----------------------
module battery() {
    color("yellow") translate([0, DY_BATTERY, 0]) {
        cylinder(d=D_BATTERY, h = Z_BATTERY + EPS2);
    }
}

module switch(extend=false)
{
    D_OUTER_TOP = 13.8;
    D_INNER_TOP = 11.0;
    H_TOP       =  1.5;
    H_BODY      = 18;   // approx. connections below.
    H = extend ? H_TOP + 5 : H_TOP;

    color("yellow") {
        rotate([-90, 0, 0]) {
            translate([0, 0, Y_SWITCH]) {
                cylinder(h=H, d=D_OUTER_TOP+1);
                translate([0, 0, -H_BODY]) {
                    cylinder(h = H_BODY, d=D_SWITCH);
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


module speakerBass22()
{
    color("yellow") {
        cylinder(h=H_SPKR_PLASTIC + EPS, d=D_SPKR_PLASTIC);
        translate([0, 0, H_SPKR_PLASTIC])
            cylinder(h=H_SPKR_METAL, d=D_SPKR_METAL);
    }
}


// Resuable parts -----------------------------------
module powerPortRing(outer, t, dz, dzToPort)
{    
    difference() {
        tube(h=dz, do=outer, di=outer-t);
        translate([0, 0, dzToPort]) {
            rotate([0,90,0]) cylinder(h=100, d=D_PORT);
        }
    }
}
