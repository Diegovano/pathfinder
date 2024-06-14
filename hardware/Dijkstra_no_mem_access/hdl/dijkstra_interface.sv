
`timescale 1ps/1ps

`include "constants.v"
// `define DEFAULT_MAX_NODES 1024 //maximum number of nodes in the graph
// `define DEFAULT_INDEX_WIDTH 10 // Width of the index
// `define DEFAULT_VALUE_WIDTH 32
// `define DEFAULT_MADDR_WIDTH 32
// `define DEFAULT_MDATA_WIDTH 16


// Interface module for the hardware implementation of Dijkstra's algorithm
// parameters are set to fit the need of the quartus system nios2 custom instruction specification

module DijkstraInterface
#(
	parameter MAX_NODES=`DEFAULT_MAX_NODES, //=32 nodes
	parameter INDEX_WIDTH=`DEFAULT_INDEX_WIDTH, //=5 bits
    parameter VALUE_WIDTH=`DEFAULT_VALUE_WIDTH //=32 bits
)
(
	input wire reset,
	input wire start, 
	input wire clock,

	input wire[7:0] select_n, //choose the mode of instruction

    input wire[31:0] dataa, 
    input wire[31:0] datab,
	// n = 0: write to edge cache
		// dataa[15:0] = from node
		// dataa[31:16] = to node
		// datab = edge weight
	// n = 1: read from previous vector cache
		// dataa[15:0] = node index
	// n = 2: compute dijkstra
		// dataa[15:0] = from node
		// dataa[31:16] = to node
		// datab[15:0] = number of nodes
	// n = 3: reset to edge cache
		// n = m: w = 0
		// n != m: w = -1
	// n = 4: reset previous vector cache to all -1

	output reg[31:0] result,
	// n = 0: write to edge cache
		// result = 0xdeadbeef
	// n = 1: read from previous vector cache
		// result = previous vector value
	// n = 2: compute dijkstra
		// result = shortest distance

	output reg ready
);

reg edge_cache_write;
reg edge_cache_read;
reg edge_cache_reset;
reg [15:0] edge_cache_from_node;
reg [15:0] edge_cache_to_node;
wire edge_cache_ready;
wire [VALUE_WIDTH-1:0] edge_cache_read_data;

EdgeCache
#(
	.MAX_NODES(MAX_NODES),
	.INDEX_WIDTH(INDEX_WIDTH),
	.VALUE_WIDTH(VALUE_WIDTH)
)
edge_cache(
	.reset(edge_cache_reset),
	.clock(clock),
	.from_node(edge_cache_from_node),
	.to_node(edge_cache_to_node),
	.read_enable(edge_cache_read), //TODO:fix this
	.write_enable(edge_cache_write),
	.write_data(datab),
	.ready(edge_cache_ready),
	.edge_value(edge_cache_read_data)
);

reg dijkstra_enable;
wire [15:0] dijkstra_ec_from_node;
wire [15:0] dijkstra_ec_to_node;
wire dijkstra_ready;
wire [31:0] dijkstra_result;

DijkstraTop
#(
	.MAX_NODES(MAX_NODES),
	.INDEX_WIDTH(INDEX_WIDTH),
	.VALUE_WIDTH(VALUE_WIDTH)
)
dijkstra_top(
	.reset(start),
	.clock(clock),
	.enable(dijkstra_enable),
	.source(dataa[15:0]),
	.destination(dataa[31:16]),
	.number_of_nodes(datab[15:0]),
	.ec_ready(edge_cache_ready),
	.ec_edge_value(edge_cache_read_data),
	.ec_query(edge_cache_read),
	.ec_from_node(dijkstra_ec_from_node),
	.ec_to_node(dijkstra_ec_to_node),
	.ready(dijkstra_ready),
	.shortest_distance(dijkstra_result)
);


typedef enum {IDLE, RUNNING} Main_State ;
Main_State state = IDLE;
Main_State next_state = IDLE;

always_ff @(posedge clock) begin
	state <= next_state;
end

always_comb begin
	// initialize value to prevent latch
	edge_cache_write = 0;
	edge_cache_reset = 0;
	dijkstra_enable = 0;
	edge_cache_from_node = 0;
	edge_cache_to_node = 0;
	ready = 0;
	result = 32'hdeadbeef;
	next_state = state;
	if (reset) begin
		next_state = IDLE;
		edge_cache_reset = 1;
	end
	case (state) 
		IDLE: begin
			if (start) begin
				next_state = RUNNING;
			end
		end
		RUNNING: begin
			case(select_n)
				0: begin
					edge_cache_write = 1;
					edge_cache_from_node = dataa[15:0];
					edge_cache_to_node = dataa[31:16];
					result = 32'h0000beef;
					ready = edge_cache_ready;
					if (edge_cache_ready) begin
						next_state = IDLE;
					end
				end
				1: begin
					result = 0;//TODO: read from previous vector cache
					ready = 1;
					next_state = IDLE;
				end
				2: begin
					dijkstra_enable = 1;
					result = dijkstra_result;
					ready = dijkstra_ready;
					edge_cache_from_node = dijkstra_ec_from_node;
					edge_cache_to_node = dijkstra_ec_to_node;
					if (dijkstra_ready) begin
						next_state = IDLE;
					end
				end
				3: begin
					edge_cache_reset = 1;
					ready = edge_cache_ready;
					if (edge_cache_ready) begin
						next_state = IDLE;
					end
				end
				4: begin
					ready = 1;
					next_state = IDLE;
				end
				default: begin
					result = 32'hdeadbeef;
					next_state = IDLE;
				end
			endcase
		end
	endcase
end




endmodule