include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>
use <holder.scad>

DRAW_HOLDER  = true;
DRAW_BODY    = true;
DRAW_EMITTER = false;

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
    
    DZFA = zLenOfBaffles(N_BAFFLES+1, H_BUTTRESS) + M_BAFFLES_BACK;
    OFFSET = 2;
    intersection() {
        innerSpace();
        translate([0, 0, DZFA]) 
            forwardAdvanced(D_AFT, 50, OFFSET, D_AFT, M_POWER - DZFA, M_SWITCH - DZFA);
    }

    // Connection
    translate([0, 0, M_BAFFLES_BACK]) mirror([0, 0, -1])
        keyJoint(JOINT_DZ, D_AFT, D_AFT - JOINT_T, false, 0);    

}

color("yellow") translate([0, 0, M_BAFFLES_BACK]) battery(D_FORE, "18500");
*color("red") translate([0, 0, M_POWER]) rotate([-90, 0, 0]) cylinder(h=16, d=12);
*color("red") translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=16, d=12);