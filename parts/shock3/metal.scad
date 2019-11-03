include <dim.scad>
use <../shapes.scad>

$fn = 40;

module segment(x0, z0, x1, z1)
{
    hull() {
        translate([x0, 0, z0]) rotate([-90, 0, 0]) cylinder(h=100, d=3.175);
        translate([x1, 0, z1]) rotate([-90, 0, 0]) cylinder(h=100, d=3.175);
    }
}

module metalArt()
{
    color("silver") {
        difference() {
            tube(h=120, do=D_OUTER, di=D_INNER, $fn=80);

            W = 18.5;
            translate([-W/2, 0, WINDOW_0_START]) 
                cube(size=[W, 100, WINDOW_0_DZ]);
            translate([-W/2, 0, WINDOW_1_START]) 
                cube(size=[W, 100, WINDOW_1_DZ]);
        }
    }

    color("gold") {
        DO_TUBE = 1.25 * 25.4;
        THICK = 0.032;
        DI_TUBE = DO_TUBE - THICK * 2 * 25.4;

        //translate([0, 20, 0])
        difference() {
            intersection() {
                translate([0, R_INNER - DO_TUBE/2, M_METAL_ART]) {
                    tube(h=DZ_METAL_ART, do=DO_TUBE, di=DI_TUBE);
                }
                W = 24;
                translate([-W/2, Y_METAL_ART, M_METAL_ART])
                    cube(size=[W, 100, DZ_METAL_ART]);
            }    

            W = 18.5;
            R = W / 2;
            SPACE = 1.5;
            T = 3.175;
            TH = 3.175 / 2;

            segment(R - T - SPACE, WINDOW_0_START, R - T - SPACE, WINDOW_0_START + WINDOW_0_DZ);
            segment(R - T - SPACE, WINDOW_1_START, R - T - SPACE, WINDOW_1_START + WINDOW_1_DZ);

            

            segment(-R + T + SPACE, WINDOW_0_START, -R + T + SPACE, WINDOW_0_START + WINDOW_0_DZ);
            segment(-R + T + SPACE, WINDOW_1_START, -R + T + SPACE, WINDOW_1_START + WINDOW_1_DZ);

/*
            *translate([-6, 0, M_CRYSTAL_START]) 
                cube(size=[12, 100, DZ_CRYSTAL_SECTION]);
            translate([-W/2, 0, WINDOW_0_START]) 
                cube(size=[W, 100, WINDOW_0_DZ]);
            translate([-W/2, 0, WINDOW_1_START]) 
                cube(size=[W, 100, WINDOW_1_DZ]);
*/                
        }
    }
}


metalArt();