$fn = 100;

difference(){
    linear_extrude(height=3, center=false){
        import("case.dxf", layer="top_grid");
    }
    linear_extrude(height=4, center=true){
        import("case.dxf", layer="groove");
    }
}


translate([0,0,-2.5]){
    difference(){
        linear_extrude(height=5, center=true){
            import("case.dxf", layer="top_wall");
        }
        linear_extrude(height=6, center=true){
            import("case.dxf", layer="groove");
        }
    }
}
translate([0,0,-7]){
    linear_extrude(height=7, center=false){
        import("case.dxf", layer="top_ridge");
    }
}
