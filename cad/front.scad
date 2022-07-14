$fn = 100;

part=100/6;
border=3;
thick=3;
numbers = ["32", "16", "8", "4", "2", "1"];

difference(){
    translate([-border/2, -(part+border)/2, 0]){
        cube([6*part+border, part+border, thick]);
    }

    for(i = [0 : 1 : 5]){
        translate([i*part+part/2, 0, -0.1]){
            linear_extrude(thick-0.3+0.1){
                text(numbers[i], 8, valign="center", halign="center");
            }
        }
    }
}

for(dist = [part : part : 5*part]){
    translate([dist, 0, -3.5]){
        cube([1.5, part, 7], center=true);
    }
}
