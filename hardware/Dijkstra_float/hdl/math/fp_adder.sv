module fp_adder (
    input [31:0] a,       // 32-bit floating point number a
    input [31:0] b,       // 32-bit floating point number b
    output [31:0] result  // 32-bit floating point result
);

    // Extract fields of the floating point numbers
    wire sign_a = a[31];
    wire sign_b = b[31];
    wire [7:0] exp_a = a[30:23];
    wire [7:0] exp_b = b[30:23];
    wire [23:0] mant_a = {1'b1, a[22:0]}; // implicit leading 1
    wire [23:0] mant_b = {1'b1, b[22:0]}; // implicit leading 1

    // Compare exponents and align mantissas
    wire [7:0] exp_diff;
    wire [23:0] aligned_mant_a;
    wire [23:0] aligned_mant_b;
    wire [7:0] exp_result;

    assign exp_diff = (exp_a > exp_b) ? (exp_a - exp_b) : (exp_b - exp_a);
    assign exp_result = (exp_a > exp_b) ? exp_a : exp_b;

    assign aligned_mant_a = (exp_a > exp_b) ? mant_a : (mant_a >> exp_diff);
    assign aligned_mant_b = (exp_b > exp_a) ? mant_b : (mant_b >> exp_diff);

    // Add or subtract mantissas based on signs
    wire [24:0] mant_sum;
    assign mant_sum = (sign_a == sign_b) ? (aligned_mant_a + aligned_mant_b) : 
                                            (aligned_mant_a - aligned_mant_b);

    // Determine result sign and mantissa
    wire result_sign;
    wire [23:0] result_mant;
    assign result_sign = (mant_sum[24]) ? sign_a : sign_b;
    assign result_mant = (mant_sum[24]) ? mant_sum[23:0] : ~mant_sum[23:0] + 1;

    // Normalize the result
    wire [7:0] final_exp;
    wire [22:0] final_mant;

    assign final_exp = exp_result; //+ mant_sum[24]; //TODO: this might be buggy, temporary fix
    assign final_mant = result_mant[22:0];

    // Construct the final result
    assign result = {result_sign, final_exp, final_mant};

endmodule
