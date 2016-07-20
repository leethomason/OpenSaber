use <threads.scad>;

// LED holder for Shock LE
// Metric       27mm pitch=1.5

FACES = 360;
EPSILON = 0.1;
VERSION_MAJOR = 3;
VERSION_MINOR = 1;

// Inches. I hate imperial.
D_RING      = 1.311;    // correct
D_WALL      = 1.220;    // correct
D_LED       = 0.75;     // correct
D_THREAD    = 1.05;     // VERIY
H_RING      = 0.280;    // measured at 279
H_INNER     = 0.110;
H_LED       = 0.700;    // correct

module tube(height, rOuter, rInner) {
    difference() {
        cylinder(h=height, r=rOuter, $fn=FACES);
        cylinder(h=height+EPSILON, r=rInner, $fn=FACES);
    }
}

module tubeD(height, dOuter, dInner) {
    difference() {
        cylinder(h=height, r=dOuter/2, $fn=FACES);
        cylinder(h=height, r=dInner/2, $fn=FACES);
    }
}

difference() {
    union() 
    {
        // Top cap
        translate([0, 0, H_LED]) {
            //tubeD(H_RING, D_RING, D_LED);
            difference() {
                cylinder(h=H_RING, r=D_RING/2, $fn=FACES);
                cylinder(h=H_RING, r=D_LED/2, $fn=FACES);
                cylinder(h=(H_RING - H_INNER), r=D_THREAD/2, $fn=FACES);
            }
        };

        // LED
        difference() {
            cylinder(h=H_LED, r=D_WALL/2, $fn=FACES);
            scale(1/25.4) {
            metric_thread(diameter=27, length=H_LED*25.4, pitch=1.5, internal=true);
            }
        }
    }
    // Version encoding.
    // Major
    translate([0,0,0.6]) {
        for(i = [0:(VERSION_MAJOR-1)]) {
            rotate([30*i,90,0]) {
                cylinder(h=2, r=0.04, $fn=20);
            }
        }
    }
    // Minor
    translate([0,0,0.4]) {
        for(i = [0:(VERSION_MINOR-1)]) {
            rotate([30*i,90,0]) {
                cylinder(h=2, r=0.04, $fn=20);
            }
        }
    }
}