module fp_comparator (
    input [31:0] a,       // 32-bit floating point number a
    input [31:0] b,       // 32-bit floating point number b
    output reg lt        // a < b
);

    // Extract sign, exponent, and mantissa
    wire sign_a = a[31];
    wire sign_b = b[31];
    wire [7:0] exp_a = a[30:23];
    wire [7:0] exp_b = b[30:23];
    wire [22:0] mant_a = a[22:0];
    wire [22:0] mant_b = b[22:0];

    always @(*) begin
        // Initialize outputs
        lt = 0;
        // Compare sign bits
        if (sign_a != sign_b) begin
            if (sign_a > sign_b) begin
                lt = 1;
            end 
        end else begin
            // Signs are the same, compare magnitudes
            if (exp_a != exp_b) begin
                if ((exp_a < exp_b && sign_a == 0) || (exp_a > exp_b && sign_a == 1)) begin
                    lt = 1;
                end 
            end else begin
                // Exponents are the same, compare mantissas
                if (mant_a != mant_b) begin
                    if ((mant_a < mant_b && sign_a == 0) || (mant_a > mant_b && sign_a == 1)) begin
                        lt = 1;
                    end 
                end 
            end
        end
    end
endmodule
