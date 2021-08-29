include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>

$fn = 60;

KEYJOINT = 8;
KEYJOINT_T = 4.5;
EPS = 0.01;

TACTILE_X = 12 + 0.2; // space for contacts
TACTILE_Z = 12 + 0.2;
TACTILE_DY = 6.5;

M_FORE = M_MC + (N_BAFFLES * 2 + 1) * DZ_BUTTRESS;

PCB_W = 14;
PCB_Y = D_INNER/2 - 8.5;
PCB_Z = M_BOLT_1 + 4.0 - M_FORE;

SLACK = 0.0;
JOINT = 6.0 + SLACK;        // distance to the joint
DX_JOINT = 3.0 - SLACK;
DZ_JOINT = 5.0 - SLACK * 2;
SECTION = 11.0;     // length of the section

TOP_W = 8.0;

echo("M_BOLT_0, M_SWITCH, M_PORT, M_BOLT_1 = ", M_BOLT, M_SWITCH, M_PORT, M_BOLT_1);
echo("M_PCB, DX_PCB, DZ_PCB = ", M_FORE, PCB_W, PCB_Z);

module bridge(dz)
{
    intersection()
    {
        cylinder(h=400, d=D_INNER);
        union() {
            polygonXY(dz, [
                [-D_INNER/2, -D_INNER/2],
                [-D_INNER/2, D_INNER/2],
                [0, D_INNER/2]
            ]);

            polygonXY(dz, [
                [D_INNER/2, -D_INNER/2],
                [D_INNER/2, D_INNER/2],
                [0, D_INNER/2]
            ]);
        }
    }
}

module pcb()
{
    translate([-PCB_W/2, -20, 0]) cube(size=[PCB_W, 40, JOINT]);
    translate([-PCB_W/2 + DX_JOINT, -20, JOINT]) cube(size=[PCB_W - DX_JOINT*2, 40, DZ_JOINT]);
    translate([-PCB_W/2, -20, JOINT + DZ_JOINT]) cube(size=[PCB_W, 40, PCB_Z - (JOINT + DZ_JOINT)]);
}

difference() {
    union() {
        translate([0, 0, M_MC]) {
            for(i=[0:N_BAFFLES]) {
                translate([0, 0, i*DZ_BUTTRESS*2])  {
                    cylinder(h=DZ_BUTTRESS, d=D_INNER);
                    if (i < N_BAFFLES) {
                        bottomRail = ((i+2)%4) != 0;
                        bridgeAndRail(2, D_INNER, DZ_BUTTRESS, bottomRail=bottomRail);
                    }
                }
            }
        }
    }
    translate([0, 0, M_MC]) {
        // Cut the top
        translate([-TOP_W/2, 0, 0]) //DZ_BUTTRESS*3]) 
            cube(size=[TOP_W, 100, 200]);

        // Cut the bottom
        BW = 12.0;
        translate([-BW/2, -100, DZ_BUTTRESS*3]) cube(size=[BW, 100, 200]);
    }
    translate([-MC_X/2, -2, M_MC]) 
        cube(size=[MC_X, MC_Y, MC_Z + 10]);
    translate([0, -0.5 * (D_INNER - D_BATTERY) / 2.0, M_BATTERY]) 
        cylinder(h=DZ_BATTERY, d=D_BATTERY);
}

difference() {
    SLACK = 0.1;
    translate([0, 0, M_FORE])
        cylinder(h=SECTION, d=D_INNER);        
    translate([0, 0, M_FORE - EPS]) pcb();
}

*union() {
    color("red") translate([0, 0, M_BOLT]) rotate([-90, 0, 0]) cylinder(h=10, d=3);
    color("red") translate([0, 0, M_SWITCH]) rotate([-90, 0, 0]) cylinder(h=10, d=12);
    color("red") translate([0, 0, M_PORT]) rotate([-90, 0, 0]) cylinder(h=10, d=9);
    color("red") translate([0, 0, M_BOLT_1]) rotate([-90, 0, 0]) cylinder(h=10, d=3);

    difference() {
        color("green") translate([-PCB_W/2, PCB_Y, M_FORE])
            cube(size=[PCB_W, 1.6, PCB_Z]);
    }
}
*color("plum") translate([0, 0, M_FORE]) pcb();
