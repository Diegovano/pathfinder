`include "constants.v"
`timescale 1ps/1ps

module MinHeap
#(parameter MAX_NODES=`DEFAULT_MAX_NODES,
parameter INDEX_WIDTH=`DEFAULT_INDEX_WIDTH,
parameter VALUE_WIDTH=`DEFAULT_VALUE_WIDTH)
(
	input wire reset,
	input wire clock,

	input wire set_en,

	// Which nodes have been visited??
	input wire[MAX_NODES-1:0] visited_vector,

	// Return highest priority node/distance
	output reg [INDEX_WIDTH-1:0] sc_min_index,
	output reg [VALUE_WIDTH-1:0] sc_min_value,
	output reg min_ready,

	input wire [VALUE_WIDTH-1:0] dist_vector[MAX_NODES-1:0],
	input wire visit_vector_true
);

// Comb logic to get min
wire [INDEX_WIDTH-1:0] heap[2*MAX_NODES-2:0];

// Find min index/value
`define ITEMS (2*MAX_NODES-start)/2
integer i;
wire [INDEX_WIDTH-1:0] min_index;
wire [VALUE_WIDTH-1:0] min_value;
generate
	genvar k;
	for(k=0;k<MAX_NODES;k=k+1)
		begin: find_min
			assign heap[k] = k;
		end
endgenerate
generate
	genvar start;
	for(start=0;start<2*MAX_NODES-2;start=(2*MAX_NODES+start)/2)
	begin: find_min3
		genvar i;
		for(i=start;i<start+`ITEMS;i=i+2)
		begin: find_min2

			wire lt;
			fp_comparator heap_lt(
				.lt(lt),
				.a(dist_vector[heap[i]]),
				.b(dist_vector[heap[i+1]])
			);
			assign heap[MAX_NODES+i/2] =
				visited_vector[heap[i]] != `UNVISITED?
					heap[i+1]:
					visited_vector[heap[i+1]] != `UNVISITED?
						heap[i]:
						lt? 
							heap[i]:
							heap[i+1];
		end
	end
endgenerate

// Clock divider by 4
logic [1:0] clk_div = 2'b00;


integer countdown;
reg heap_ready=0;//signal to communicate between clock domains


`define CYCLES_TO_WAIT 5 
//don't change this arbitrarily, 
//it's the number of cycles to wait before setting the ready signal
// which need to match with the difference between clock domains


always @(posedge clock) begin
	if(reset || set_en || visit_vector_true) begin
		clk_div <= 2'b00;
	end
	else begin
		clk_div <= clk_div + 1;
	end
	if(reset || set_en || visit_vector_true)
		begin
			
			heap_ready	<= 0;
			min_ready <= 0; //blocking assignment: TODO: change to non-blocking
			countdown <= `CYCLES_TO_WAIT; 

		end
	else if(countdown > 0)
		begin
			countdown <= countdown - 1;
		end
	else
		begin
			heap_ready <= 1;
			min_ready <= 1;

		end
	
end

wire clock_divided4 = clk_div[1];

always @(posedge clock_divided4) begin
	if(heap_ready)
		begin
			sc_min_index <= heap[2*MAX_NODES-2];
			sc_min_value <= dist_vector[heap[2*MAX_NODES-2]];
		end

end
endmodule
