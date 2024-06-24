
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
	input wire clock_enable,

	input wire[7:0] select_n, //choose the mode of instruction

    input wire[31:0] dataa, 
    input wire[31:0] datab,
	// n = 0: write to edge cache
		// dataa[15:0] = from node
		// dataa[31:16] = to node
		// datab = edge weight
	// n = 1: read edge cache
		// dataa[15:0] = node index
	// n = 2: compute dijkstra
		// dataa[15:0] = from node
		// dataa[31:16] = to node
		// datab[15:0] = number of nodes
	// n = 3: read previous vector cache
		// dataa[15:0] = node index

	output reg[31:0] result,
	// n = 0: write to edge cache
		// result = 0xdeadbeef
	// n = 1: read from edge cache
		// result = previous vector value
	// n = 2: compute dijkstra
		// result = shortest distance
	// n = 3: read from previous vector cache
		// result = previous vector value

	output reg ready,

	//avalon slave signals
	input wire[15:0] slave_address,
	input wire slave_read,
	input wire slave_write,
	output logic[31:0] slave_readdata,
	input wire[31:0] slave_writedata,
	output reg slave_waitrequest,

	//interupt signals
	output reg interrupt_sender_irq,

	input wire clock_sink,
	input wire reset_sink
);

reg edge_cache_write;
reg edge_cache_read;
reg edge_cache_reset;
reg [15:0] edge_cache_from_node;
reg [15:0] edge_cache_to_node;
wire edge_cache_ready;
logic [31:0] edge_cache_write_data;
wire [VALUE_WIDTH-1:0] edge_cache_read_data;
reg ec_addr_sel = 0;

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
	.DMA_Slave_address(slave_address),
	.add_sel(ec_addr_sel),
	.read_enable(edge_cache_read), //TODO:fix this
	.write_enable(edge_cache_write),
	.write_data(edge_cache_write_data),
	.ready(edge_cache_ready),
	.edge_value(edge_cache_read_data)
);

reg dijkstra_enable;
wire [15:0] dijkstra_ec_from_node;
wire [15:0] dijkstra_ec_to_node;
wire dijkstra_ec_read;
wire dijkstra_ready;
wire [31:0] dijkstra_result;
wire [INDEX_WIDTH-1:0] visited_vector_data;
wire dijkstra_start;
assign dijkstra_start = start && select_n == 2;

DijkstraTop
#(
	.MAX_NODES(MAX_NODES),
	.INDEX_WIDTH(INDEX_WIDTH),
	.VALUE_WIDTH(VALUE_WIDTH)
)
dijkstra_top(
	.reset(dijkstra_start),
	.clock(clock),
	.enable(dijkstra_enable),
	.source(dataa[15:0]),
	.destination(dataa[31:16]),
	.number_of_nodes(datab[15:0]),
	.ec_ready(edge_cache_ready),
	.ec_edge_value(edge_cache_read_data),
	.ec_query(dijkstra_ec_read),
	.ec_from_node(dijkstra_ec_from_node),
	.ec_to_node(dijkstra_ec_to_node),
	.ready(dijkstra_ready),
	.shortest_distance(dijkstra_result),
	.visited_vector_read_address(dataa[15:0]),
	.visited_vector_data(visited_vector_data)
);


typedef enum {IDLE, RUNNING} Main_State ;
Main_State state = IDLE;
Main_State next_state = IDLE;

//assign slave_readdata = 32'hdeadbeef;

always_ff @(posedge clock) begin
	state <= next_state;
end

always_comb begin
	// initialize value to prevent latch
	edge_cache_write = 0;
	edge_cache_reset = 0;
	edge_cache_read = 0;
	edge_cache_write_data = 32'hdeadbeef;
	dijkstra_enable = 0;
	edge_cache_from_node = 0;
	edge_cache_to_node = 1;
	ready = 0;
	result = 32'hdeadbeef;
	next_state = state;

	slave_waitrequest = 0;
	slave_readdata = 32'hdeadbeef;
	if (reset) begin
		next_state = IDLE;
		edge_cache_reset = 1;
	end
	case (state) 
		IDLE: begin
			edge_cache_write = slave_write;
			edge_cache_read = slave_read;
			edge_cache_write_data = slave_writedata;
			edge_cache_from_node = {12'b0,slave_address[3:0]};
			edge_cache_to_node = {12'b0,slave_address[7:4]};
			ec_addr_sel = 0;

			
			slave_readdata = edge_cache_read_data;
			slave_waitrequest = 0;

			if (start) begin
				next_state = RUNNING;
			end
		end
		RUNNING: begin
			edge_cache_write_data = datab;
			ec_addr_sel = 1;
			case(select_n)
				0: begin //writing to cache
					edge_cache_write = 1;
					edge_cache_from_node = dataa[15:0];
					edge_cache_to_node = dataa[31:16];
					result = 32'h0000beef;
					ready = edge_cache_ready;
					if (edge_cache_ready && clock_enable) begin
						next_state = IDLE;
					end
				end
				1: begin //reading from cache
					edge_cache_from_node = dataa[15:0];
					edge_cache_to_node = dataa[31:16];
					edge_cache_read = 1;
					result = edge_cache_read_data;
					ready = edge_cache_ready && clock_enable;
					if (edge_cache_ready && clock_enable) begin
						next_state = IDLE;
					end
				end
				2: begin
					dijkstra_enable = 1;
					edge_cache_read = dijkstra_ec_read;
					result = dijkstra_result;
					ready = dijkstra_ready && clock_enable;
					edge_cache_from_node = dijkstra_ec_from_node;
					edge_cache_to_node = dijkstra_ec_to_node;
					if (dijkstra_ready && clock_enable) begin
						next_state = IDLE;
					end
				end
				3: begin //TODO Buggy
					ready = clock_enable;
					result = visited_vector_data;
					if (clock_enable) begin
						next_state = IDLE;
					end
				end
				default: begin
					result = 32'hdeadbeef;
					ready = 1;
					if (clock_enable) begin
						next_state = IDLE;
					end
				end
			endcase
		end
	endcase
end




endmodule