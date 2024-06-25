module fp_adder (
    input logic [31:0] a,
    input logic [31:0] b,
    output logic [31:0] result
);

    // Split the input floating-point numbers into sign, exponent, and significand
    logic sign_a, sign_b;
    logic [7:0] exponent_a, exponent_b;
    logic [23:0] significand_a, significand_b;

    // Intermediate signals
    logic [7:0] exponent_diff;
    logic [24:0] significand_a_shifted, significand_b_shifted;
    logic [24:0] significand_sum;
    logic sign_result;
    logic [7:0] exponent_result;
    logic [7:0] final_exponent_result;
    logic [23:0] significand_result;
    logic [24:0] significand_result_extended;

    // Unpack the floating-point numbers
    always_comb begin
        sign_a = a[31];
        sign_b = b[31];
        exponent_a = a[30:23];
        exponent_b = b[30:23];
        significand_a = {1'b1, a[22:0]}; // Implicit leading 1
        significand_b = {1'b1, b[22:0]}; // Implicit leading 1
    end

    // Align the significands
    always_comb begin
        if (exponent_a > exponent_b) begin
            exponent_diff = exponent_a - exponent_b;
            significand_a_shifted = significand_a;
            significand_b_shifted = significand_b >> exponent_diff;
            exponent_result = exponent_a;
        end else begin
            exponent_diff = exponent_b - exponent_a;
            significand_a_shifted = significand_a >> exponent_diff;
            significand_b_shifted = significand_b;
            exponent_result = exponent_b;
        end
    end

    // Add or subtract the significands
    always_comb begin
        if (sign_a == sign_b) begin
            significand_sum = significand_a_shifted + significand_b_shifted;
            sign_result = sign_a;
        end else begin
            if (significand_a_shifted > significand_b_shifted) begin
                significand_sum = significand_a_shifted - significand_b_shifted;
                sign_result = sign_a;
            end else begin
                significand_sum = significand_b_shifted - significand_a_shifted;
                sign_result = sign_b;
            end
        end
    end

    // Normalize the result
    always_comb begin
        if (significand_sum[24] == 1) begin
            significand_result_extended = significand_sum;
            final_exponent_result = exponent_result + 1;
        end else begin
            significand_result_extended = significand_sum << 1;
            final_exponent_result = exponent_result;
        end

        // Normalize to fit in 23 bits
        significand_result = significand_result_extended[23:1];
    end

    // Pack the result
    always_comb begin
        result = {sign_result, final_exponent_result, significand_result[22:0]};
    end

endmodule
