include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>
use <holder.scad>

DRAW_HOLDER = false;
DRAW_BODY   = true;

$fn = 80;

Z_OFFSET = 3;

DX_POWER = 11;
DY_POWER = 9;
DZ_POWER = 14;

DY_PCB = 0;
Y_POWER = -yAtX(DX_POWER/2, D_AFT/2) + 1;

ROT = -25;
Y_TWEAK = -5;

module key(out)
{
    KL = 10;
    
    intersection() 
    {
        tube(h=KL, do=D_AFT, di=out ? D_AFT-4 : 0);
        translate([-4, -20, 0])
            cube(size=[8, 20, KL]);
    }
}

if (DRAW_HOLDER) {
    holder();
}

if (DRAW_BODY) {

    BATTERY_BAFFLES = nBafflesNeeded(H_BUTTRESS);
    //N_BAFFLES = floor(DZ_HOLDER / (H_BUTTRESS*2));
    N_BAFFLES = BATTERY_BAFFLES + 1;

    echo("Battery baffles=", BATTERY_BAFFLES);
    echo("nBaffles=", N_BAFFLES);
    
    intersection() {
        union() {
            cylinder(h=DZ_CHASSIS, d=D_AFT);
            translate([0, 0, DZ_CHASSIS]) cylinder(h=20, d=D_FORWARD);
        }
        difference() {
            union() {
                for(i=[0:N_BAFFLES - 1]) {
                    translate([0, 0, i*H_BUTTRESS*2 + DZ_PCB]) {
                        oneBaffle(
                            D_AFT,
                            H_BUTTRESS,
                            battery=false,
                            mc=true,
                            bridge=(i < N_BAFFLES-1) && (i != 7),
                            scallop=false,
                            cutout=false,
                            mcSpace=false
                        );
                    }
                }
                // Side walls
                D_BATTERY = 19.0;
                translate([D_BATTERY/2, -10, DZ_CHASSIS - H_BUTTRESS]) cube(size=[10, 20, H_BUTTRESS*4]);
                mirror([1, 0, 0]) translate([D_BATTERY/2, -10, DZ_CHASSIS - H_BUTTRESS]) cube(size=[10, 20, H_BUTTRESS*4]);

                // Buff up the back.
                Z = (N_BAFFLES * 2 - 2) * H_BUTTRESS + DZ_PCB;
                translate([-20, 0, Z]) cube(size=[40, 40, H_BUTTRESS]);
            }
            translate([0, 0, DZ_PCB])
                battery(D_FORWARD);

            // remove forward cruft
             translate([-10, -20, DZ_CHASSIS]) cube(size=[20, 8, 100]);

            // flat bottom
            translate([-20, -20, 0])
                cube(size=[40, 5, 100]);
            
            // Connection
            translate([0, 0, DZ_PCB]) 
                key(false);
        }
    }
}

*color("yellow") translate([0, 0, DZ_PCB]) battery(D_FORWARD);