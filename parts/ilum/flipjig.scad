
H = 1.25 * 25.4 * 0.5 - 0.8;    // height for pcb

DX = 3 * 25.4 - 5;
DY = 2 * 25.4 - 5;
D = 13.0;
PILLAR = 5;

$fn = 80;

translate([-DX/2, -DY/2, 0]) cylinder(h=H, d=D);
translate([ DX/2, -DY/2, 0]) cylinder(h=H, d=D);
translate([-DX/2,  DY/2, 0]) cylinder(h=H, d=D);
translate([ DX/2,  DY/2, 0]) cylinder(h=H, d=D);

translate([-DX/2, -DY/2 - PILLAR/2, 0]) cube(size=[DX, PILLAR, PILLAR]);
translate([-DX/2,  DY/2 - PILLAR/2, 0]) cube(size=[DX, PILLAR, PILLAR]);
translate([-DX/2 - PILLAR/2, -DY/2, 0]) cube(size=[PILLAR, DY, PILLAR]);
translate([ DX/2 - PILLAR/2, -DY/2, 0]) cube(size=[PILLAR, DY, PILLAR]);
