include <dim.scad>

EPS = 0.1;
EPS2 = EPS * 2;

module bridgeSide(h)
{
    BRIDGE_X = 13;
    Y = 5;
    BRIDGE_DY = 3;

    hull() {
        // Skinny end
        translate([BRIDGE_X, Y - BRIDGE_DY, -h - EPS])
            cube(size=[2, BRIDGE_DY, h + EPS2]);
        // Fat end (achieve a 45 degree angle)
        translate([BRIDGE_X,  Y - BRIDGE_DY - h, 0])
            cube(size=[2, BRIDGE_DY + h, EPS]);             
    }
}


function buttressZ(i) = M_AFT_STOP_FRONT + (i+1) * DZ_BUTTRESS + i * H_BUTTRESS;

module railCube()
{
    translate([9, -12, 0]) cube(size=[3, 2, H_BUTTRESS]);
}

module buttress(    trough=16,
                    h=H_BUTTRESS,
                    bridge=0,
                    rail=false)
{

    if (rail == true) {
        intersection() {
            translate([0, 0, -bridge - EPS]) cylinder(h=bridge + EPS2, d = D_INNER);
            union() {
                translate([9, -12, -bridge - EPS]) cube(size=[10, 2, bridge + EPS2]);
                mirror([1, 0, 0]) translate([9, -12, -bridge - EPS]) cube(size=[10, 2, bridge + EPS2]);
            }
        }
    }

    difference() {
        cylinder(h=h, d=D_INNER); 
        translate([-trough/2, 0, -EPS]) cube(size=[trough, 100, h + EPS2]);
        translate([0, 0, -EPS]) cylinder(h=h + EPS2, d=D_INNER_CORE);
    }

    if (bridge > 0) {
        bridgeSide(bridge);
        mirror([1, 0, 0]) bridgeSide(bridge);
    }
}
