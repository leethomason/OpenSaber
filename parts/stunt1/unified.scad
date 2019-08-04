include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>
use <holder.scad>

DRAW_HOLDER = false;
DRAW_BODY   = true;
DRAW_EMITTER = false;

DZ_BODY = (EMITTER == "closed") ? (DZ_TOTAL - emitterZ()) : DZ_TOTAL;

$fn = 80;
EPS = .01;
EPS2 = 2 * EPS;

module flatBottom() {
    translate([-20, -D_AFT/2 - 5, -20]) cube(size=[40, 5.5, 500]);
}

if (DRAW_HOLDER) {
    difference() {
        union() {
            holder();
            if (DZ_RING > 0) {
                difference() {
                    translate([0, 0, -DZ_RING]) tube(h=DZ_RING, do=D_RING, di=D_AFT - 4);
                    translate([-6, -D_AFT/2, -DZ_RING-EPS2]) 
                        cube(size=[10, 10, DZ_RING+EPS2]);
                }
            }
        }
        flatBottom();
    }
}

module innerSpace()
{   
    if (M_STEPDOWN > 0) {
        cylinder(h=M_STEPDOWN, d=D_AFT);
        translate([0, 0, M_STEPDOWN]) cylinder(h=400, d=D_FORE);
    } 
    else {
        cylinder(h=DZ_AFT, d=D_AFT);
        translate([0, 0, DZ_AFT]) cylinder(h=400, d=D_FORE);
    }
}

module AAHolder(extraZ)
{
    D_BATT_HOLDER = 17.2;
    DY_BATT = 8.0;
    Y_TOT = DY_BATT + D_BATT_HOLDER / 2;
    DZ_BATT_HOLDER = 58.0;

    translate([0, R_AFT - D_BATT_HOLDER/2, 0])
        cylinder(h=DZ_BATT_HOLDER + extraZ, d=D_BATT_HOLDER);
    translate([-D_BATT_HOLDER/2, R_AFT - D_BATT_HOLDER/2 - DY_BATT, 0])
        cube(size=[D_BATT_HOLDER, DY_BATT, DZ_BATT_HOLDER + extraZ]);
}

if (DRAW_BODY) {

    BATTERY_BAFFLES = nBafflesNeeded(H_BUTTRESS, BATTERY_TYPE);
    N_BAFFLES = ceil((DZ_BODY - DZ_PCB + H_BUTTRESS)/(H_BUTTRESS*2));

    echo("Battery baffles=", BATTERY_BAFFLES);
    echo("nBaffles=", N_BAFFLES);
    
    intersection() {
        innerSpace();
        difference() {
            union() {
                for(i=[0:N_BAFFLES - 1]) {
                    REINFORCE = 4;
                    z = i*H_BUTTRESS*2 + DZ_PCB;
                    batt = i < BATTERY_BAFFLES;
                    hasEmitter = EMITTER != "none";

                    translate([0, 0, z]) {
                        oneBaffle(
                            D_AFT,
                            H_BUTTRESS,
                            battery=batt,
                            conduit = !batt,
                            mc=batt,
                            bridge=(i < N_BAFFLES-1 || hasEmitter) ? 1 : 0,
                            noBottom=i < REINFORCE,
                            bottomRail = ((i%2) == 0)
                        );
                    }
                }
                *translate([0, 0, DZ_AFT - H_BUTTRESS - EPS]) difference() {
                    tube(h=DZ_FORE + H_BUTTRESS, do=D_FORE, di=D_FORE-2);
                    translate([-50, 0, 0]) cube(size=[100, 100, 500]); 
                }

                // Connection
                translate([0, 0, DZ_PCB]) mirror([0, 0, -1])
                    keyJoint(JOINT_DZ, D_AFT, D_AFT - JOINT_T, false, 0);    

                if (EMITTER=="open") {
                    translate([0, 0, DZ_TOTAL]) {
                        dynamicHeatSinkHolder(D_FORE);
                    }
                }
                else if (EMITTER=="closed") {
                    translate([0, 0, DZ_BODY])
                        emitterBase(D_FORE);
                }
            }
            // flat bottom
            flatBottom();

            if (M_SWITCH) {
                translate([0, -50, M_SWITCH]) {
                    rotate([-90, 0, 0])
                        cylinder(h=100, d=16.4);   
                }
            }          
            if (EMITTER=="open") {
                spacer = 10;
                translate([0, 0, DZ_TOTAL - spacer]) 
                    cylinder(h=100, d=dynamicHeatSinkThread());
            }              
        }
    }
}

if (DRAW_EMITTER) {
    if (EMITTER == "closed") {
        translate([0, 0, DZ_BODY])
            emitterHolder(D_FORE);
    }
}

*color("yellow") translate([0, 0, DZ_PCB]) battery(D_FORE);
*color("red") cylinder(h=DZ_AFT, d=2.0);