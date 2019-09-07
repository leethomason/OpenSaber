include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>
use <holder.scad>

DRAW_HOLDER  = false;
DRAW_BODY    = true;

$fn = 80;
EPS = .01;
EPS2 = 2 * EPS;

N_BAFFLES = nBafflesNeeded(H_BUTTRESS, "18500");

module flatBottom() {
    translate([-20, -D_AFT/2 - 5, -20]) cube(size=[40, 5.5, 500]);
}

if (DRAW_HOLDER) {
    translate([0, 0, M_BACK]) difference() {
        holder(D_AFT, 37.0);
        flatBottom();
    }
}

module innerSpace()
{   
    cylinder(d=D_AFT, h=M_TRANSITION);
    translate([0, 0, M_TRANSITION]) cylinder(h=400, d=D_FORE);
}

if (DRAW_BODY) {
    translate([0, 0, M_BAFFLES_BACK]) {
        echo("nBaffles=", N_BAFFLES);
            baffleMCBattery(D_AFT, N_BAFFLES, H_BUTTRESS, nPostBaffles=1);
    }

    MOUNT = 5.5;
    DZFA = zLenOfBaffles(N_BAFFLES+1, H_BUTTRESS) + M_BAFFLES_BACK;

    intersection() {
        innerSpace();
        difference() {
            union() {
                translate([0, 0, M_SWITCH-8])
                    switchRing(D_AFT, 3, 20, 8);
                translate([0, 0, M_POWER-6])
                    powerPortRing(D_AFT, 3, 14, 6);

            }
            flatBottom();
            for(i=[0:2])
                translate([0, 0, M_POWER + i * 8]) capsule(120, 60, _mirror = true);
        }
    }

    // Front sled
    intersection() {
        OVERLAP = 1;
        innerSpace();
        union() {
            difference() {
                translate([0, 0, M_TRANSITION - OVERLAP]) {                
                    tube(h=M_HEATSINK_BACK - M_TRANSITION + OVERLAP, do=D_AFT, di=D_FORE-3);
                }
                THETA = 25;
                for(i=[0:1]) {
                    translate([0, 0, 4 + M_TRANSITION + i * 8]) {
                        capsule(90+THETA, 90-THETA, _mirror = true);
                        capsule(-THETA, THETA);
                        capsule(180-THETA, 180+THETA);
                    }
                }
                flatBottom();
            }
        }
    }
    DZ_THREAD = 10.0;
    translate([0, 0, M_HEATSINK_BACK - DZ_THREAD]) {
        tube(h=DZ_THREAD, di=dynamicHeatSinkThread(), do=D_FORE);
    }

    // Connection
    translate([0, 0, M_BAFFLES_BACK]) mirror([0, 0, -1])
        keyJoint(JOINT_DZ, D_AFT, D_AFT - JOINT_T, false, 0);    

}

*color("yellow") translate([0, 0, M_BAFFLES_BACK]) battery(D_FORE, "18500");
*color("red") translate([0, 0, M_POWER]) rotate([-90, 0, 0]) cylinder(h=16, d=12);
*color("red") translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=16, d=12);