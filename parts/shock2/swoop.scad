$fn=40;

module arch(dx=12, dz=1)
{
    intersection() 
    {
        OFFSET_Y = dx/12;
        DY = dx / 3;
        D = dx * 4 / 3;
        
        translate([0, OFFSET_Y, 0])
            scale([1,2,1]) 
                translate([-dx/4,0,0])
                    cylinder(h=dz, d=D);
        translate([0, OFFSET_Y, 0])
            scale([1,2,1]) 
                translate([dx/4,0,0]) 
                    cylinder(h=dz, d=D);
        translate([-dx/2, 0, 0]) 
            cube(size=[dx, 40, dz]);
    }
}

*arch(12, 4);

difference() 
{
    cube(size=[10, 30, 80]);
    translate([12, 5, 10]) 
    {
        for(i=[0:1]) {
            translate([0, 1, 1 + i*22]) 
                rotate([0, -90, 0])
                    arch(dz=12);
            translate([0, 20, 12 + i*22]) 
                rotate([0, -90, 0])
                    rotate([0, 0, 180])
                        arch(dz=12);
        }
    }
}
