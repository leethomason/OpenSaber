$fn = 120;

DZ_PCB = 13;
DZ_EXTRA = 2;
DZ = DZ_PCB + DZ_EXTRA;

D = 20 - 0.4;
T = 3;
T_PCB = 2;

difference()
{
    cylinder(h=DZ, d=D);
    cylinder(h=DZ, d=D - T);
    translate([-T_PCB/2, -20, 0]) cube(size=[T_PCB, 40, DZ_PCB]);
}