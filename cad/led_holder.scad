$fn = 100;

difference(){   
    linear_extrude(height=100){
        import("led_holder.dxf", layer="profile");
    }
    translate([0, 0, 97]){
        linear_extrude(height=3.1){
            import("led_holder.dxf", layer="cable");
        }
    }
}

