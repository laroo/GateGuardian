// Import and extrude the GateGuardian box SVG
// Creates a 1mm thick wall with 10mm height and 0.5mm floor

$fn = 100;

union() {
    // Floor - 0.5mm thick
    linear_extrude(height = 0.5) {
        import("GateGuardian_box_only.svg");
    }
    
    // Walls - 1mm thick, 10mm height (starting from floor top)
    translate([0, 0, 0.5])
        linear_extrude(height = 9.5) {
            difference() {
                import("GateGuardian_box_only.svg");
                offset(delta = -1) {
                    import("GateGuardian_box_only.svg");
                }
            }
        }


    color("red") linear_extrude(height = 8) {
        import("GateGuardian_drill_only.svg");
    }

    
    // Drill holes - 5 cylinders at exact positions from SVG
    // SVG viewBox: 300 300 142085.01 77209.998
    // Radius in SVG: 1770.5713 units (convert to mm by dividing by 1000)
    drill_radius = 4 ;
    top_radius = 2;
    drill_height = 7;
    svg_height = 77209.998;
    
    translate([0, -0.6, 0]) {
        // Convert SVG coordinates to mm and flip Y-axis (SVG Y goes down, OpenSCAD Y goes up)
        translate([(5799.4961 - 300) / 1000, (svg_height - (5880.9316 - 300)) / 1000, 0])
            cylinder(h = drill_height, r1 = drill_radius, r2=top_radius);
        
        translate([(45168.711 - 300) / 1000, (svg_height - (70018.875 - 300)) / 1000, 0])
            cylinder(h = drill_height, r1 = drill_radius, r2=top_radius);

        translate([(68659.102 - 300) / 1000, (svg_height - (24936.012 - 300)) / 1000, 0])
            cylinder(h = drill_height, r1 = drill_radius, r2=top_radius);
        
        translate([(132804.52 - 300) / 1000, (svg_height - (23039.188 - 300)) / 1000, 0])
            cylinder(h = drill_height, r1 = drill_radius, r2=top_radius);
        
        translate([(125817.86 - 300) / 1000, (svg_height - (62406.035 - 300)) / 1000, 0])
            cylinder(h = drill_height, r1 = drill_radius, r2=top_radius);
    }
}
