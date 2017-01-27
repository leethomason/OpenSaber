use <../vents.scad>

EPS = 0.01;
EPS2 = EPS * 2;

module hexShape(w, h, d)
{
	// Translate to cube coordinates.
	translate([w/2, 0, 0]) rotate([-90, -90, 0]) lowerVent2(w, h, d);
}

module beam(w, h, d)
{
    
    innerW = w * 0.75;
    innerH = h * 0.75;
    deltaW = w - innerW;
    deltaH = h - innerH;

    nSide = floor(d / w );
    space = (d - w * nSide) / nSide;
    echo("nSide", nSide, "space", space);

    difference() {
        cube(size=[w, h, d]);
        translate([deltaW/2, deltaH/2, -EPS]) hexShape(innerW, innerH, d + EPS2);
       
        for(i=[0:nSide-1]) 
    	{
        	//echo(i);
        	translate([w + EPS, deltaH/2, space / 2 + w*i]) rotate([0, -90, 0]) hexShape(innerW, innerH, w + EPS2);
        }
    }
}

*difference() 
{
	cube(size=[4, 6, 20]);
	translate([0.5 + 1.5, 0.5, -EPS]) rotate([-90, -90, 0]) lowerVent2(3, 5, 20 + EPS2);
	for(i=[0:4]) {
		translate([-10, 0.5, 2 + 4*i]) rotate([-90, 0, 0]) lowerVent2(3, 5, 20);
	}
}

//rotate([-90, -90, 0]) lowerVent2(3, 8, 10);
//rotate([-90, 0, 0]) lowerVent2(3, 8, 1);

beam(3, 6, 22);

