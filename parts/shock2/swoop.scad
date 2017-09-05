$fn=40;

module poly()
{
    intersection() 
    {
        {
        DX = 6;
        DY = 4;
        D = 16;
        
        scale([1,2,1]) cylinder(h=20, d=D);
        scale([1,2,1]) translate([-DX,0,0]) cylinder(h=20, d=D);
        //translate([-DX/2,DY,0]) cylinder(h=20, d=D);
        translate([-20, -1, -20]) cube(size=[40, 40, 40]);
        }
    }
    /*
    linear_extrude(height=20) {
        p = [[0,0], [10,0], 
            [9, 5], [7.5,8],
            [5,10],
            [2.5,8], [1,5]];
        //bezier(0, 0, 0, p);
        //echo(p[0]);
        polygon(points=p);
        
        minkowski() {
            //p = [[0,0], [10,0], [7,7], [5,10], [3,7]];
            p = [[0,0], [10,0], [5,10]];
            polygon(points=p);
            circle(radius=1, $fn=20);
        }
    }
    */
}

difference() 
{
    cube(size=[10, 30, 80]);
    translate([12, 5, 10]) 
    {
        rotate([0, -90, 0])
        translate([1, 1, 0]) poly();
    }
}
