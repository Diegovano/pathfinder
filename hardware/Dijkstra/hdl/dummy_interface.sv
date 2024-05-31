
`timescale 1ps/1ps


module DummyInterface
(
    //input wire main_reset, // reset signal for the whole system, currently only a placeholder
	input wire algorithm_reset,
	input wire algorithm_clock,
	input wire algorithm_enable,

	input wire mem_clock, //these are not actually used, but required for the interface
	input wire mem_reset,

    input wire[31:0] dummy_base_address, // dataa
    input wire[31:0] datab, // datab, 

	output wire mem_read_enable,
	output wire mem_write_enable,

	input wire mem_write_ready,
	input wire mem_read_ready,
    input wire [1:0] mem_write_response, // required for interface, not actually used

	output wire [32-1:0] mem_addr,
	input wire [16-1:0] mem_read_data,
	output wire [16-1:0] mem_write_data,

	output reg[31:0] shortest_distance,

	output reg ready = 0,	

	input wire wait_request
);
// initial begin
// 	ready = 0;
// 	@ (posedge algorithm_clock);
// 	ready = 1;
// end
//initial ready = 0;
int state = 0;

always @(posedge algorithm_clock) begin
	if (state == 0) begin
		ready <= 1;
		state <= 1;
	end
	else begin
		ready <= 0;
		state <= 0;
	end
	shortest_distance <= datab + dummy_base_address;
end



endmodule