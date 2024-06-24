`timescale 1ns/1ps

module tb_floating_point_adder();
    reg [31:0] a, b;
    reg [31:0] result;
    reg clk = 0;

    // Instantiate the floating point adder
    fp_adder uut (
        .a(a),
        .b(b),
        .result(result)
    );

    always #10000 clk = ~clk;

    // Test vectors
    initial begin

        @(posedge clk);
        // Test case 1
        a = 32'h3f800000; // 1.0 in IEEE 754
        b = 32'h40000000; // 2.0 in IEEE 754
        $display("a = %h, b = %h, result = %h", a, b, result);
        if (result !== 32'h40400000) // 3.0 in IEEE 754
            $display("Test case 1 failed");

        @(posedge clk);
        // Test case 2
        a = 32'h40400000; // 3.0 in IEEE 754
        b = 32'h40800000; // 4.0 in IEEE 754
        $display("a = %h, b = %h, result = %h", a, b, result);
        if (result !== 32'h40a00000) // 7.0 in IEEE 754
            $display("Test case 2 failed");
        
        @(posedge clk);
        // Additional test cases can be added here

        $finish;
    end
endmodule
