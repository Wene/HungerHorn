$fn = 100;

difference(){
    union(){
        linear_extrude(height=3){
            import("case.dxf", layer="top_grid");
        }
        translate([0,0,-5]){
            linear_extrude(height=5){
                import("case.dxf", layer="top_ridge");
            }
        }
        translate([0,0,-12]){
            linear_extrude(height=12){
                import("case.dxf", layer="top_cable");
            }
        }
    }

    translate([0,0,-0.1]){
        linear_extrude(height=2.1){
            import("case.dxf", layer="groove");
        }
    }
}
