$fn = 100;

difference(){
    
    union(){
        translate([0,0,-3]){
            linear_extrude(height=3, center=false){
                import("case.dxf", layer="base");
            }
        }
        linear_extrude(height=10, center=false){
            import("case.dxf", layer="base_wall");
        }
        linear_extrude(height=7.5, center=false){
            import("case.dxf", layer="base_ridge");
        }
        linear_extrude(height=8, center=false){
            import("case.dxf", layer="speaker_support_01");
        }
        linear_extrude(height=15, center=false){
            import("case.dxf", layer="speaker_support_02");
        }
    }
    
    translate([0,0,-2]){
        linear_extrude(height=16, center=false){
            import("case.dxf", layer="groove");
        }
    }
}




