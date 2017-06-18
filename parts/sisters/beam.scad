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

    hexH = h * 0.75;
    hexW = hexH / 2;

    proposedStep = hexW * 1.4;

    // subract out space for the ends.
    nHex = floor(d / proposedStep);
    step = d / nHex;
    space = step - hexW;

    //echo("w", w, "h", h, "d", d, "nHex", nHex, "step", step);

    difference() {
        cube(size=[w, h, d]);
        for(i=[0:nHex-1]) 
    	{
            x = w + EPS;
            y = (h - hexH) / 2;
            z = space / 2 + step * i;

        	translate([x, y, z]) 
                rotate([0, -90, 0])
                    hexShape(hexW, hexH, w + EPS2);
        }
    }
}

beam(3, 7, 25);

