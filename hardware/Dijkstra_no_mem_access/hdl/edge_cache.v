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
	output reg [VALUE_WIDTH-1:0] edge_value
);


// Where the row is stored after we pull it from the graph
reg [VALUE_WIDTH-1:0] row_cache[MAX_NODES-1:0][MAX_NODES-1:0];
integer i;
integer j;

assign edge_value = row_cache[from_node][to_node];

always @(posedge clock, posedge reset)
begin
	if(reset)
		begin
			for(i=0; i<MAX_NODES; i=i+1)
				for(j=0; j<MAX_NODES; j=j+1)
					if(i==j)
						row_cache[i][j] = 0;
					else
						row_cache[i][j] = `INFINITY;
			ready = 1'b1;
		end
	else if(read_enable)
		begin
			ready = 1'b1;
		end
	else if (write_enable)
		begin
			row_cache[from_node][to_node] = write_data;
			ready = 1'b1;
		end
	else
		ready = 1'b0;
end
	
endmodule
