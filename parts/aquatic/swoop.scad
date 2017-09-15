$fn=40;

module archOuter(dx=12, sy=1, dz=1)
{
    intersection() 
    {
        OFFSET_Y = 0; //dx/12;
        DY = dx / 3;
        D = dx * 4 / 3;
        
        translate([0, OFFSET_Y, 0])
            scale([1,sy*2,1]) 
                translate([-dx/4,0,0])
                    cylinder(h=dz, d=D);
        translate([0, OFFSET_Y, 0])
            scale([1,sy*2,1]) 
                translate([dx/4,0,0]) 
                    cylinder(h=dz, d=D);
        translate([-dx/2, 0, 0]) 
            cube(size=[dx, dx*2*sy, dz]); // dx*2*sy is a guess
    }
}

module arch(dx=12, sy=1, dz=1, inset=2)
{   
    difference() {
        archOuter(dx, sy, dz);
        translate([0,inset,-1])
            archOuter(
                dx-inset*2, 
                sy*(1-inset * 0.05), 
                dz+2);
    }
}

*arch(16, 0.8, 4);

*for(i=[0:3]) {
    translate([i*8.5,0,0])
        //arch(18, 0.7, 4, 3);
        arch(14, 1.0, 4, 2);
}

*difference() 
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
