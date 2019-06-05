include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>
use <holder.scad>

DRAW_HOLDER = true;
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

module innerSpace()
{
    cylinder(h=DZ_AFT, d=D_AFT);
    translate([0, 0, DZ_AFT]) cylinder(h=DZ_FORE, d=D_FORE);
}

if (DRAW_BODY) {

    BATTERY_BAFFLES = nBafflesNeeded(H_BUTTRESS);
    N_BAFFLES = BATTERY_BAFFLES + 1;

    echo("Battery baffles=", BATTERY_BAFFLES);
    echo("nBaffles=", N_BAFFLES);
    
    intersection() {
        innerSpace();
        difference() {
            union() {
                for(i=[0:BATTERY_BAFFLES]) {
                    last = i == BATTERY_BAFFLES;
                    translate([0, 0, i*H_BUTTRESS*2 + DZ_PCB]) {
                        oneBaffle(
                            D_AFT,
                            H_BUTTRESS,
                            battery=!last,
                            mc=true,
                            bridge=!last,
                            scallop=false,
                            cutout=false,
                            mcSpace=false
                        );
                    }
                }
            }
            // flat bottom
            translate([-20, -20, 0])
                cube(size=[40, 5, 100]);
            
            // Connection
            translate([0, 0, DZ_PCB]) 
                key(false);
        }
    }
}

*color("yellow") translate([0, 0, DZ_PCB]) battery(D_AFT);