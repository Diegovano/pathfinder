module accelerator_top #(
	parameter FOLD_FACT = 4,
	parameter CORD_ITER = 16
)
(
	input logic clk,
	input logic clk_en,
	input logic reset,
	input logic start,
	input logic[31:0] x,
	output logic done,
	output logic[31:0] y
);

typedef enum logic [2:0] {
	IDLE_STATE,
	MULTIPLY_STATE_1, // includes cordic
	MULTIPLY_STATE_2, // x^2 * cos
	ACCUMULATE_STATE,
	OUTPUT_AVAIL_STATE
} state_t;

logic start_cordic;
logic done_cordic;

logic [31:0] x_cos_out;

logic [31:0] mul_dataa_1;
logic [31:0] mul_datab_1;
logic [31:0] mul_res_1;

logic [31:0] add_dataa_1;
logic [31:0] add_datab_1;
logic [31:0] add_res_1;

state_t state, next_state;

fp_mult mult_1 (
	.clk(clk),
	.areset(reset),
	.a(mul_dataa_1),
	.b(mul_datab_1),
	.q(mul_res_1)
);

fp_add add_1 (
	.clk(clk),
	.areset(reset),
	.a(mul_res_1),
	.b(x[30:23] == 8'b0 ? {x[31:23], x[22:0] >> 1} : {x[31], x[30:23] - 1, x[22:0]}),
	.q(add_res_1)
);

cordic_top #(.FOLD_FACT(FOLD_FACT), .CORD_ITER(CORD_ITER)) my_cordic (
	.clk(clk),
	.clk_en(clk_en),
	.reset(reset),
	.start(start_cordic),
	.done(done_cordic),
	.x_ft(x),
	.y_ft(x_cos_out)
);

int count;

always_ff @ (posedge clk) begin
	if (clk_en) begin
		if (reset) state <= IDLE_STATE;
		else begin
			state <= next_state;

			count = next_state != state ? 0 : count + 1;
		end
	end
end

always_comb begin
	start_cordic = 0;
	done = 0;

	mul_dataa_1 = 0;
	mul_datab_1 = 0;

	y = 0;

	next_state = IDLE_STATE;		
		// state = next_state;
	case (state)
		default: begin // covers idle state
		// IDLE_STATE: begin
			start_cordic = 1;
			if (start) begin
				next_state = MULTIPLY_STATE_1;
			end
			else begin
				next_state = IDLE_STATE;
			end
		end

		MULTIPLY_STATE_1: begin
			start_cordic = 0;

			mul_dataa_1 = x;
			mul_datab_1 = x;

			if (done_cordic && count >= 2) begin
				next_state = MULTIPLY_STATE_2; // if mults finished and cordic finished
			end
			else next_state = MULTIPLY_STATE_1;
		end

		MULTIPLY_STATE_2: begin
			mul_dataa_1 = mul_res_1;
			mul_datab_1 = x_cos_out;

			if (count >= 2 - 1) begin
				next_state = ACCUMULATE_STATE;
			end
			else next_state = MULTIPLY_STATE_2;
		end

		ACCUMULATE_STATE: begin
			if (count >= 2 - 1) begin // prepare data for immediate release next cycle
				next_state = OUTPUT_AVAIL_STATE;
			end
			else next_state = ACCUMULATE_STATE;
		end

		OUTPUT_AVAIL_STATE: begin
			next_state = IDLE_STATE;
			done = 1;

			y = add_res_1;

		end
	endcase
end

endmodule