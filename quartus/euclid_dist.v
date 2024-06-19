// module to calculate the euclidean distance between two points
// equation used: sqrt((x2-x1)^2 + (y2-y1)^2)
module euclid_dist(
    input  wire        clk,
    input  wire        reset,
    input  wire [31:0] x1,
    input  wire [31:0] y1,
    input  wire [31:0] x2,
    input  wire [31:0] y2,
    output wire [31:0] dist
);

    // Internal variables
    wire [31:0] x_diff;
    wire [31:0] y_diff;
    wire [31:0] x_diff_sq;
    wire [31:0] y_diff_sq;
    wire [31:0] sum;
    
    
	 // subtraction 
    fp_sub x_sub
    (
        .clk(clk) ,	
        .areset(reset) ,	
        .a(x1) ,	
        .b(x2) ,	
        .q(x_diff) 	
    );
    fp_sub y_sub
    (
        .clk(clk) ,	
        .areset(reset) ,	
        .a(y1) ,	
        .b(y2) ,	
        .q(y_diff) 	
    );

    
	 // squaring
    fp_mult x_sq
    (
        .clk(clk) ,	
        .areset(reset) ,	
        .a(x_diff) ,	
        .b(x_diff) ,	
        .q(x_diff_sq) 	
    );
    fp_mult y_sq
    (
        .clk(clk) ,	
        .areset(reset) ,	
        .a(y_diff) ,	
        .b(y_diff) ,	
        .q(y_diff_sq) 	
    );
	 
    
	 // addition
    fp_add add1
    (
        .clk(clk) ,	
        .areset(reset) ,	
        .a(x_diff_sq) ,	
        .b(y_diff_sq) ,	
        .q(sum) 	
    );

    
	 // square root
    fp_sqrt sqrt1
    (
        .clk(clk) ,	
        .areset(reset) ,	
        .a(sum) ,	
        .q(dist) 	
    );

endmodule 