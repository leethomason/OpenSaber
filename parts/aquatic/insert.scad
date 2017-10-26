include <dim.scad>

$fn = 90;
H = 30;

difference() {
    cylinder(h=H, d=D_POMMEL);
    cylinder(h=H, d=D_SPKR_PLASTIC - 2);
}