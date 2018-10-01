include <dim.scad>
use <../shapes.scad>
use <util.scad>
use <../commonUnified.scad>

//$fn = 40;
$fn = 90;

DRAW_AFT = true;

N_BAFFLES = nBafflesNeeded(DZ_BUTTRESS);
DZ_BAFFLES = zLenOfBaffles(N_BAFFLES, DZ_BUTTRESS);
T = 4;

if (DRAW_AFT) {
    difference() {
        union() {
            translate([0, 0, M_AFT_BACK]) 
                speakerHolder(D_AFT_MID, DZ_SPKR_HOLDER, 1, "cls28");

            translate([0, 0, M_AFT_BACK + DZ_SPKR_HOLDER])
                baffleMCBattery(D_AFT_MID, N_BAFFLES, DZ_BUTTRESS);

            M_FRONT_BAFFLES =  M_AFT_BACK + DZ_SPKR_HOLDER + DZ_BAFFLES;
            *translate([0, 0, M_FRONT_BAFFLES])
                tube(h=1, do=D_AFT_MID, di=D_AFT_MID - T);

            translate([0, 0, M_FRONT_BAFFLES])
                pcbHolder(
                    D_AFT_MID, 
                    T,                    
                    M_MID_FRONT - M_FRONT_BAFFLES,  // arbitrary; there is more space
                    1,  // dz PCB
                    5,  // dy PCB
                    [16, 50, 22],        // fixme: guess
                    []
                );
        }
    }
}

*inner();
*translate([0, 0, M_AFT_BACK + DZ_SPKR_HOLDER])
    battery(D_AFT_MID);

*translate([-PLATE_DX/2, 0, M_MID_CENTER - PLATE_MOUNT/2])
    color("orange")
        cube(size=[PLATE_DX, 20, PLATE_MOUNT]);

*translate([0, 0, M_MID_BACK]) cylinder(h=DZ_MID, d=D_AFT_MID);
