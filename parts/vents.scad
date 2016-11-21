module lowerVent1(w, h, extrusion)
{
    h1 = h - w * 1.2;
    h2 = h;
    rotate([90, 0, 90]) {
        linear_extrude(height=extrusion) {
            polygon([[-w/2,0], [w/2,0], [w/2,h1], [0,h2], [-w/2,h1]]);
        }
    }    
}


module vent1(w, h, n, extrusion)
{
    angle = 360 / n;
    
	for(r=[0:n-1]) {
		rotate([0, 0, angle*r]) {
            lowerVent1(w, h, extrusion);
        }
    }
}

module vent2(w, h, n, extrusion)
{
    angle = 360 / n;
	for(r=[0:n-1]) {
		rotate([0, 0, angle*r]) {

            rotate([90, 0, 90]) {
                linear_extrude(height=extrusion) {
                    polygon([[0,0], [w/2,w/2], [w/2, h-w/2], [0, h], [-w/2, h-w/2], [-w/2, w/2]]);
                }
            }
        }
    }
}

module vent3(w, h, n, extrusion)
{
    h1 = 0.6 * h;
    h2 = h;
    
	W = w / 2;
	IW = W * 0.62;
	IH = h1 * 0.38;
    
    angle = 360 / n;
	for(r=[0:n-1]) {
		rotate([0, 0, angle*r]) {

            rotate([90, 0, 90]) {
                linear_extrude(height=extrusion) {
                    difference() {
                        polygon([[-W,0], [W,0], [W,h1], [0,h2], [-W,h1]]);
                        polygon([[-IW, 0], [IW, 0], [IW, IH], [-IW, IH]]);
                    }
                }
            }
        }
	}
}

module vent4(h, r, extrusion)
{
    alpha = 16;
    w = sin(alpha) * r;

	for(i=[0:11]) {
		rotate([0, 0, 30*i]) {
			rotate([90, 0, 90]) {
				linear_extrude(height=extrusion) {
					polygon([[-w/2, 0], [w/2, 0], [0, h] ]);
				}
			}
		}
		rotate([0, 0, 30*i + 15]) {
			rotate([90, 0, 90]) {
				linear_extrude(height=extrusion) {
					polygon([[-w/2, h], [0, 0], [w/2, h] ]);
				}
			}
		}
	}
}


difference() 
{
    H = 16;
    D = D_INNER;
	tube(H, D/2 - 2, D/2);	
    
	translate([0, 0, 3]) {
        //vent1(6, H-6, 10, 40);
        //vent2(6, H-6, 8, 40);
        vent3(6, H-6, 7, 40);
		//vent4(H-6, D/2, D);
	}
}
