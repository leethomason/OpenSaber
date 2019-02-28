use <../inset.scad>
use <../commonUnified.scad>
include <dim.scad>

$fn = 60;
EPS = 0.01;

DRAW_AFT = false;
DRAW_FORE = false;
DRAW_EMITTER = true;

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


if (DRAW_FORE)
{
    difference()
    {
        union() 
        {
            translate([0, 0, M_BATTERY_FRONT]) {
                difference() {
                    tube(h=M_CAPSULE_START - M_BATTERY_FRONT, do=D_INNER, di=D_INNER - 6);
                    translate([0, -1, 0]) 
                        cylinder(h=M_CAPSULE_START - M_BATTERY_FRONT, d=D_INNER - 6);
                }
            }
            translate([0, 0, M_CAPSULE_START]) {
                insetHolder(D_INNER, D_OUTER,
                    M_EMITTER - M_CAPSULE_START,
                    (M_CAPSULE_BACK - M_CAPSULE_START) + DZ_CAPSULE_NOM / 2,
                    DZ_PORT, DZ_SWITCH,
                    D_CAPSULE, DZ_BAFFLE
                );    
            }
        }
        translate([0, 0, M_BATTERY_FRONT]) {
            keyJoint(8, D_INNER + EPS*4, D_INNER - 4.0, 0.0, 0.0, true);
        }
    }

    translate([0, 0, M_EMITTER]) {
        emitterBase(D_INNER);
    }
}

if (DRAW_EMITTER) {
    translate([0, 0, M_EMITTER]) {
        emitterHolder(D_INNER);
    }
}


//translate([0, 0, M_BATTERY_BACK]) color("red") battery(D_INNER);