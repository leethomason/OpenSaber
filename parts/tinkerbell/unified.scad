include <dim.scad>
use <../shapes.scad>
use <../commonUnified.scad>

//$fn = 80; // overrides fs; num fragments
$fa = 6;   // minimum angle (default 12)
$fs = 0.8;  // minimum size (default 2)

N_BAFFLES = 8;
T = 4.0;
EPS = 0.01;
D_BATTERY = 19.0;
MM_TO_IN = 25.4;

module pillar(h, pos=true)
{
    rotate([-90, 0, 0]) difference() {
        hull() {
            cylinder(d=6.0, h=h);
            translate([-2 * (pos ? 1 : -1), 0, 0]) cylinder(d=6.0, h=h);
        }
        translate([0, 0, -EPS]) cylinder(d=1.8, h=20);
    }
}

difference() {
    union() {
        translate([0, 0, M_START])
            difference() {
                union() {
                    tubeTriTop(h = DZ_SPKR_HOLDER, do = D0, di = D0 - T*2);
                    translate([0, 0, -2]) tubeTriTop(h=2.01, do=D0 - 4, di = D0 - T*2 - 4);
                }
                translate([-50, -D0/2, -10]) cube(size=[100, D0/2 - 4.0, 100]);
            }
        translate([0, 0, M_MC])
            baffleMCBattery2(D0, N_BAFFLES, DZ_BATTERY);

        translate([0, 0, M_PCB]) {
            pcbHolder2(
                D0, T+0.5, DZ_PCB_SECTION, PCB_OFFSET, 
                [DX_PCB, DY_PILLAR_TOP, DZ_PCB],
                [
                    [22.86 / 2 - 19.685, 2.54, "buttress"],
                    [22.86 / 2 - 3.175,  2.54, "buttress"],
                    [22.86 / 2 - 19.685, 22.225, "buttress"],
                    [22.86 / 2 - 3.175,  22.225, "buttress"]
                ]
            );
        }
        echo("Front drop:", DZ_BODY - (M_PCB + DZ_PCB_SECTION));
    }

    // cutout bolt
    translate([-50, -50, M_BOLT]) cube(size=[100, 50, DZ_BOLT]);

    // side cutouts
    for(i=[0:2])
        translate([-50, 0, M_PCB + 9 + i * 5])
            baffleVent(3.5);

    hull() {
        D = 4.0;
        translate([-50,  5, M_FORE_PLATE + 12.0]) rotate([0, 90, 0]) cylinder(h=100, d=D);
        translate([-50, -5, M_FORE_PLATE + 12.0]) rotate([0, 90, 0]) cylinder(h=100, d=D);
    } 

    hull() {
        D = 4.0;
        translate([4, 0, M_FORE_PLATE + 12.0]) rotate([-90, 0, 0]) cylinder(h=100, d=D);
        translate([-4, 0, M_FORE_PLATE + 12.0]) rotate([-90, 0, 0]) cylinder(h=100, d=D);
    }
    // flatten
    translate([-20, -D0/2 - EPS - 5.0, -20]) cube(size=[40, 0.5 + 5.0, 200]);

    // LED control
    translate([-50, -D0/2, M_LED_CONTROL]) cube(size=[100, 4, DZ_LED_CONTROL]);

    // shave off pcb front plate "noise"
    translate([0, 0, M_PCB + DZ_PCB_SECTION - 0.1]) cylinder(h=1.0, d=D0);
}

// bolt
intersection() 
{
    translate([-50, -50, M_BOLT]) cube(size=[100, 50, DZ_BOLT]);
    rotate([0, 0, BOLT_ROT]) translate([0, 0, M_BOLT])
        bottomBoltRing(diameter=D0, t=T, dz=DZ_BOLT, dzToBolt=DZ_BOLT/2); 
    echo("Bolt rot:", sin(BOLT_ROT) * D_OUTER/2);
}

// LED pillars.
translate([0, 0, M_LED_CONTROL]) {
    Y = -6.5;
    H = 4.0;
    translate([-DX_LED_CONTROL/2 + 0.1 * MM_TO_IN, Y, 0.1 * MM_TO_IN]) pillar(h=H, pos=true);
    translate([-DX_LED_CONTROL/2 + 0.6 * MM_TO_IN, Y, 0.1 * MM_TO_IN]) pillar(h=H, pos=false);
    translate([-DX_LED_CONTROL/2 + 0.1 * MM_TO_IN, Y, 1.35 * MM_TO_IN]) pillar(h=H, pos=true);
    translate([-DX_LED_CONTROL/2 + 0.6 * MM_TO_IN, Y, 1.35 * MM_TO_IN]) pillar(h=H, pos=false);
}

// switch PCB holder
translate([0, -4.5, -15]) {
    DZ = 0.6 * 25.4;
    DX = 0.7 * 25.4;
    EX = 3.0;
    difference() {
        union() {
            translate([DX/2 - 2, 0, 0]) cube(size=[2, 2, DZ + EX*2]);
            mirror([-1, 0, 0]) translate([DX/2 - 2, 0, 0]) cube(size=[2, 2, DZ + EX*2]);            
        }
        translate([-DX/2, -EPS, EX]) cube(size=[DX, 10, DZ]);
    }
}

// 11mm wide
// switch to front: 5mm
// port to back: 5.5mm
M_PORT = M_PCB + 0.3 * 25.4;
M_SWITCH = M_PCB + 0.6886 * 25.4;
echo("Back:", M_PORT - 5.5, "Drop:", DZ_BODY - (M_PORT - 5.5));
echo("Front", M_SWITCH + 5.0, "Drop:", DZ_BODY - (M_SWITCH + 5.0));
echo("Bolt", M_BOLT, "Drop:", DZ_BODY - M_BOLT);

*translate([0, 0, M_FRONT]) color("red") cylinder(d=D0, h=1.0);