H = 10.0;
$fn = 120;

difference() {
    union() {
        cylinder(h=H, d=10.5);
        cylinder(h=3, d=17.0);
    }
    cylinder(h=20, d=3.2);
}
