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

    W = h / 2;
    endPad = W / 4;
    nSide = floor((d - endPad*2) / W);
    space = (d - endPad*2) / nSide;

    difference() {
        cube(size=[w, h, d]);
        for(i=[0:nSide-1]) 
    	{
            N = 4;
            VAR_H = [1.0, 0.8, 1.0, 0.8]; //, 0.9, 0.6, 1.0, 0.8];
            VAR_Y = [0.0, 0.2, 0.0, 0.0]; // 0.2, 0.1, 0.0, 0.0];

            y = deltaH / 2 + innerH * VAR_Y[i%N];

        	translate([w + EPS, y, space/2 + space*i]) 
                rotate([0, -90, 0])
                    hexShape(innerW, innerH * VAR_H[i%N], w + EPS2);
        }
    }
}

beam(3, 7, 25);

