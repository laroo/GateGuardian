// Import and extrude the GateGuardian box SVG
// Creates a 1mm thick wall with 10mm height and 0.5mm floor

$fn = 100;

// Define drill hole parameters
drill_radius = 4;
top_radius = 2;
drill_height = 7;
svg_height = 77209.998;
cutout_diameter = 2;
cutout_depth = 5;
box_height = 10;

// Define drill hole positions
drill_positions = [
    [(5799.4961 - 300) / 1000, (svg_height - (5880.9316 - 300)) / 1000],
    [(45168.711 - 300) / 1000, (svg_height - (70018.875 - 300)) / 1000],
    [(68659.102 - 300) / 1000, (svg_height - (24936.012 - 300)) / 1000],
    [(132804.52 - 300) / 1000, (svg_height - (23039.188 - 300)) / 1000],
    [(125817.86 - 300) / 1000, (svg_height - (62406.035 - 300)) / 1000]
];

difference() {
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
        
        // Drill holes - 5 conical cylinders
        translate([0, -0.6, 0]) {
            for (pos = drill_positions) {
                translate([pos[0], pos[1], 0])
                    cylinder(h = drill_height, r1 = drill_radius, r2 = top_radius);
            }
        }
    }
    
    // Cut out 2mm diameter cylinders, 5mm deep from the top
    translate([0, -0.6, 0]) {
        for (pos = drill_positions) {
            translate([pos[0], pos[1], box_height - cutout_depth])
                cylinder(h = cutout_depth + 0.1, r = cutout_diameter / 2);
        }
    }
}
