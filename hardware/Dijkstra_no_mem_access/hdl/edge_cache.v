`include "constants.v"
`timescale 1ps/1ps

// Directly polls memory for edges
// and hands out values
module EdgeCache
#(
	parameter MAX_NODES=`DEFAULT_MAX_NODES,
	parameter INDEX_WIDTH=`DEFAULT_INDEX_WIDTH,
	parameter VALUE_WIDTH=`DEFAULT_VALUE_WIDTH
)
(
	input wire reset,
	input wire clock,
	
	// address
	input wire[INDEX_WIDTH-1:0] from_node,//row
	input wire[INDEX_WIDTH-1:0] to_node,//column

	// Input signals for when we want to query an edge
	input wire read_enable, //diven by n=2 dijkstra
	input wire write_enable, //diven by n=0 
	input wire [VALUE_WIDTH-1:0] write_data, //diven by n=0
	
	// When we have the requested edge value, set ready high and edge_value to
	// the value from memory
	output reg ready,
	output wire [VALUE_WIDTH-1:0] edge_value
);

EdgeCacheMem 
#(
	.MAX_NODES(MAX_NODES),
	.INDEX_WIDTH(INDEX_WIDTH),
	.VALUE_WIDTH(VALUE_WIDTH)
)
edge_cache_mem(
	.clock(clock),
	.address({to_node[4:0], from_node[4:0]}),
	.write_enable(write_enable),
	.write_data(write_data),
	.edge_value(edge_value)
);

always @(posedge clock, posedge reset)
begin
	if(reset)
		begin
			// TODO: reset the cache
			ready = 1'b1;
		end
	else if(read_enable)
		begin
			ready = 1'b1;
		end
	else if (write_enable)
		begin
			ready = 1'b1;
		end
	else
		ready = 1'b0;
end
	
endmodule
