include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>
use <holder.scad>

DRAW_HOLDER = false;
DRAW_BODY   = true;

$fn = 80;
EPS = .01;

if (DRAW_HOLDER) {
    holder();
}

module innerSpace()
{
    cylinder(h=DZ_AFT, d=D_AFT);
    translate([0, 0, DZ_AFT]) cylinder(h=DZ_FORE, d=D_FORE);
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

    BATTERY_BAFFLES = nBafflesNeeded(H_BUTTRESS);
    N_BAFFLES = floor((DZ_TOTAL - DZ_PCB + H_BUTTRESS)/(H_BUTTRESS*2));
    REINFORCE = 4;

    echo("Battery baffles=", BATTERY_BAFFLES);
    echo("nBaffles=", N_BAFFLES);
    
    intersection() {
        innerSpace();
        difference() {
            union() {
                for(i=[0:N_BAFFLES - 1]) {
                    z = i*H_BUTTRESS*2 + DZ_PCB;
                    batt = (i < BATTERY_BAFFLES) && !USE_AA;

                    translate([0, 0, z]) {
                        oneBaffle(
                            D_AFT,
                            H_BUTTRESS,
                            battery=batt,
                            mc=i >= REINFORCE,
                            bridge=(z < DZ_AFT - H_BUTTRESS*2) ? 1 : 0,
                            scallop=false,
                            cutout=i < REINFORCE,
                            mcSpace=false
                        );
                    }
                }
                translate([0, 0, DZ_AFT - H_BUTTRESS - EPS]) difference() {
                    tube(h=DZ_FORE + H_BUTTRESS, do=D_FORE, di=D_FORE-2);
                    translate([-50, 0, 0]) cube(size=[100, 100, 500]); 
                }
            }
            //translate([0, 0, DZ_PCB])
            //    battery(D_FORE);

            // flat bottom
            translate([-20, -20, 0])
                cube(size=[40, 5, 100]);

            if (M_SWITCH) {
                translate([0, -50, M_SWITCH]) {
                    rotate([-90, 0, 0])
                        cylinder(h=100, d=16.4);   
                }
            }          
            if (ADD_EMITTER) {
                spacer = 10;
                translate([0, 0, DZ_TOTAL - spacer]) 
                    cylinder(h=100, d=dynamicHeatSinkThread());
            }  
            if (USE_AA) {
                translate([0, 0, DZ_PCB - EPS]) AAHolder(3);
            }
        }
    }
    // Connection
    translate([0, 0, DZ_PCB]) mirror([0, 0, -1])
        keyJoint(JOINT_DZ, D_AFT, D_AFT - JOINT_T, 0.1, 0, true);    

    if (ADD_EMITTER) {
        translate([0, 0, DZ_TOTAL]) {
            dynamicHeatSinkHolder(D_FORE);
        }
    }
}

*color("yellow") translate([0, 0, DZ_PCB]) battery(D_FORE);
*color("red") translate([0, 0, DZ_PCB - EPS]) AAHolder();