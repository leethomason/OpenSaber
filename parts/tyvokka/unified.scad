use <../inset.scad>
use <../commonUnified.scad>
use <../shapes.scad>
include <dim.scad>

$fn = 80;
EPS = 0.01;

DRAW_AFT = true;
DRAW_FORE = false;
DRAW_EMITTER = false;

module flatBottom()
{
    translate([-50, -R_INNER-2, -50]) {
        cube(size=[100, 2.5, 400]);
    }
}

if (DRAW_AFT) {
    translate([0, 0, M_SPEAKER_BACK])
        speakerHolder(D_INNER, M_BATTERY_BACK - M_SPEAKER_BACK, 3, "cls28");

    translate([0, 0, M_BATTERY_BACK]) {
        dz = M_BATTERY_FRONT - M_BATTERY_BACK;
        nBaffles = nBafflesNeeded(DZ_BAFFLE) + 1;   // padding.
        dzAllBaffles = zLenOfBaffles(nBaffles, DZ_BAFFLE);

        baffleMCBattery(
            D_INNER,
            nBaffles,
            DZ_BAFFLE   
        );
    }
    translate([0, 0, M_BATTERY_FRONT]) {
        keyJoint(8, D_INNER, D_INNER - 4.0, 0.1, 0.0, true);
    }
}

module dotstarCutout()
{
    N_DOTSTAR = 4;
    Z_DOTSTAR_LEN = dotstarStripZLen(N_DOTSTAR);
    DOTSTAR_STRIP_XZ = 12.8;
    
    // Railing (so handy) and dotstar cutout.
    translate([R_INNER - 3.0, -DOTSTAR_STRIP_XZ/2, M_CAPSULE_CENTER - Z_DOTSTAR_LEN/2]) {
        cube(size=[0.8, DOTSTAR_STRIP_XZ, Z_DOTSTAR_LEN]);
    }

    OFFSET = 2;
    translate([R_INNER - 3.0, -DOTSTAR_STRIP_XZ/2 + OFFSET, M_CAPSULE_CENTER - Z_DOTSTAR_LEN/2]) {
        xRoofCube([10, DOTSTAR_STRIP_XZ - OFFSET, Z_DOTSTAR_LEN + 4]);
    }

    // Extra cutout for wiring.
    translate([R_INNER - 6.0, -DOTSTAR_STRIP_XZ/2+1, M_CAPSULE_CENTER + 12]) {
        xRoofCube([10, DOTSTAR_STRIP_XZ-4, 4]);
    }
}


if (DRAW_FORE)
{
    difference()
    {
        union() 
        {
            translate([0, 0, M_BATTERY_FRONT]) {
                difference() {
                    H = M_CAPSULE_BACK - M_BATTERY_FRONT;
                    tube(h=H, do=D_INNER, di=D_INNER - 8);
                    translate([0, -1, 0]) 
                        cylinder(h=H, d=D_INNER - 6);
                }
            }
            translate([0, 0, M_CAPSULE_BACK]) {
                insetHolder(D_INNER, D_OUTER,
                    M_EMITTER - M_CAPSULE_BACK,
                    DZ_CAPSULE_NOM / 2,
                    DZ_PORT, DZ_SWITCH,
                    D_CAPSULE, DZ_BAFFLE
                );    
            }
            translate([0, 0, M_EMITTER]) {
                emitterBase(D_INNER);
            }
        }
        translate([0, 0, M_BATTERY_FRONT]) {
            keyJoint(8, D_INNER + EPS*4, D_INNER - 4.0, 0.0, 0.0, true);
        }
        mirror([-1, 0, 0]) dotstarCutout();
        flatBottom();
        
        translate([0, 0, M_EMITTER])
            for(r=[0:5])
                rotate([0, 0, 60*r])
                    capsule(-10, 10, 2);
    }
    //color("red") dotstarCutout();
}


if (DRAW_EMITTER) {
    translate([0, 0, M_EMITTER]) {
        emitterHolder(D_INNER);
    }
    echo("Saber stop=", M_PHYSICAL_STOP, "z=", M_EMITTER + emitterZ());
    if (M_EMITTER + emitterZ() >= M_PHYSICAL_STOP) {
        echo("** Physical stop exceeded. **");
    }
}


//translate([0, 0, M_BATTERY_BACK]) color("red") battery(D_INNER);