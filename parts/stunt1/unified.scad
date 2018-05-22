include <dim.scad>
use <../commonUnified.scad>
use <../shapes.scad>

$fn = 80;

C = 19.05 / 2;

pcbHolder(D_AFT, 
          4, 
          26,   // dz
          2,    // dz to pcb
          0,    // dy pcb
          [22, 50, 22],
          [
            [3.81 - C,   2.54, "buttress"],
            [19.05 - C,  2.54, "buttress"],
            [3.81 - C,  19.05, "buttress"],
            [19.05 - C, 19.05, "buttress"]
          ] );