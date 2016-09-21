include <dim.scad>

intersection()
{
	cylinder(d=D_INNER, h=40, $fn=180);
	translate([-D_INNER/2, -11, 0]) {
		cube(size=[5, 6, 29.8]);
	}

}