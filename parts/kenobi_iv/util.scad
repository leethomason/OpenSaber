include <dim.scad>

module inner()
{
    translate([0, 0, M_AFT_BACK])
        cylinder(h=DZ_AFT, d=D_AFT);
    translate([0, 0, M_MID_BACK])
        cylinder(h=DZ_MID, d=D_MID);
    translate([0, 0, M_FORE_BACK])
        cylinder(h=DZ_FORE, d=D_FORE);
}