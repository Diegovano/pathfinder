`include "constants.v"
`timescale 1ps/1ps

// should infer to be a block ram
module EdgeCacheMem
#(
	parameter MAX_NODES=`DEFAULT_MAX_NODES,
	parameter INDEX_WIDTH=`DEFAULT_INDEX_WIDTH,
	parameter VALUE_WIDTH=`DEFAULT_VALUE_WIDTH
)
(
	input wire clock,
	//[9:5] to node, [4:0] from node
	input wire[2*INDEX_WIDTH-1:0] address, 

	// Input signals for when we want to query an edge
	input wire write_enable, //diven by n=0 
	input wire [VALUE_WIDTH-1:0] write_data, //diven by n=0
	
	output reg [VALUE_WIDTH-1:0] edge_value
);


// Where the row is stored after we pull it from the graph
reg [VALUE_WIDTH-1:0] mem [MAX_NODES**2-1:0];

always @(posedge clock)
begin
	if(write_enable)
		mem[address] <= write_data;
	edge_value <= mem[address];
end
	
endmodule
