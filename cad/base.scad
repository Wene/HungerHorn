$fn = 100;

wall = 100/6;
speaker_height = 18;
speaker_base = 12;
support_height = wall - (speaker_height - speaker_base);

difference(){
    
    union(){
        translate([0,0,-3]){
            linear_extrude(height=3){
                import("case.dxf", layer="base");
            }
        }
        linear_extrude(height=wall){
            import("case.dxf", layer="base_wall");
        }
        linear_extrude(height=wall-5.5){
            import("case.dxf", layer="base_ridge");
            import("case.dxf", layer="led_holder");
        }
        linear_extrude(height=support_height){
            import("case.dxf", layer="speaker_support_out");
        }
    }
    
    translate([0,0,-2]){
        linear_extrude(height=wall+3){
            import("case.dxf", layer="groove");
        }
    }
    translate([0,0,support_height-speaker_base]){
        linear_extrude(height=speaker_base+0.1){
            import("case.dxf", layer="speaker_support_in");
        }
    }
}




