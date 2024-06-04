`include "constants.v"
`timescale 1ps/1ps

// Top level module for the HW implementation of Dijkstra's algorithm
module DijkstraTop
#(
	parameter MAX_NODES=`DEFAULT_MAX_NODES,
	parameter INDEX_WIDTH=`DEFAULT_INDEX_WIDTH,
	parameter VALUE_WIDTH=`DEFAULT_VALUE_WIDTH,
	parameter MADDR_WIDTH=`DEFAULT_MADDR_WIDTH,
	parameter MDATA_WIDTH=`DEFAULT_MDATA_WIDTH
)
(
	input wire reset,
	input wire clock,
	input wire enable,

	input wire[INDEX_WIDTH-1:0] source,
	input wire[INDEX_WIDTH-1:0] destination,

	input wire[INDEX_WIDTH-1:0] number_of_nodes,
	input wire[MADDR_WIDTH-1:0] base_address,

	output reg mem_read_enable,
	output reg mem_write_enable,

	input wire mem_write_ready,
	input wire mem_read_ready,

	output wire [MADDR_WIDTH-1:0] mem_addr,
	input wire [MDATA_WIDTH-1:0] mem_read_data,
	output wire [MDATA_WIDTH-1:0] mem_write_data,

	output wire[VALUE_WIDTH-1:0] shortest_distance,

	output reg ready,
	input wire wait_request,
	output wire isFINAL_STATe
);

// Reset components at our will
reg controlled_ec_reset;
reg controlled_pq_reset;
reg ec_mem_read_enable;
reg writer_mem_write_enable;

// Keep track of paths
reg [INDEX_WIDTH-1:0] prev_vector[MAX_NODES-1:0];

// States for the FSM
typedef enum {RESET_STATE, READY_STATE, V0, V1, V2, V3, V4, V5, WRITE_STATE, FINAL_STATE} State ;
State state = FINAL_STATE;
State next_state = FINAL_STATE;

// Visited nodes
integer number_of_unvisited_nodes;
reg [MAX_NODES-1:0] visited_vector;
reg visit_vector_true;

wire [VALUE_WIDTH-1:0] dist_vector[MAX_NODES-1:0];
assign shortest_distance = dist_vector[destination];

// Just for for loops
integer i;

// The node we're visiting
reg[INDEX_WIDTH-1:0] current_node;
reg[VALUE_WIDTH-1:0] current_node_value;

// Reduction variable
//reg[VALUE_WIDTH-1:0] alt;


reg pq_set_distance;
reg[INDEX_WIDTH-1:0] pq_index;
reg[VALUE_WIDTH-1:0] pq_distance_to_set;
wire[VALUE_WIDTH-1:0] pq_distance_read;
wire[INDEX_WIDTH-1:0] min_distance_node_index;
wire[VALUE_WIDTH-1:0] min_distance_node_value;
wire min_ready;

PriorityQueue #(.MAX_NODES(MAX_NODES), .INDEX_WIDTH(INDEX_WIDTH), .VALUE_WIDTH(VALUE_WIDTH))
	priority_queue (
		controlled_pq_reset,
		clock,
		pq_set_distance, //pq set_en
		pq_index,
		visited_vector,
		pq_distance_to_set,
		pq_distance_read,
		min_distance_node_index,
		min_distance_node_value,
		min_ready,
		dist_vector,
		visit_vector_true
	);


reg ec_query;
reg[INDEX_WIDTH-1:0] ec_from_node;
reg[INDEX_WIDTH-1:0] ec_to_node;
wire ec_ready;
wire [VALUE_WIDTH-1:0] ec_edge_value;

EdgeCache
#(
	.MAX_NODES(MAX_NODES),
	.INDEX_WIDTH(INDEX_WIDTH),
	.VALUE_WIDTH(VALUE_WIDTH),
	.MADDR_WIDTH(MADDR_WIDTH),
	.MDATA_WIDTH(MDATA_WIDTH)
)
	edge_cache(
		controlled_ec_reset,
		clock,
		base_address,
		number_of_nodes,
		ec_query,
		ec_from_node,
		ec_to_node,
		mem_addr,
		mem_read_data,
		ec_mem_read_enable,
		mem_read_ready,
		ec_ready,
		ec_edge_value,
		wait_request
	);

reg writer_enable=0;
wire writer_ready;
reg[MADDR_WIDTH-1:0] writer_address;

Writer 
#(
	.MAX_NODES(MAX_NODES),
	.INDEX_WIDTH(INDEX_WIDTH),
	.VALUE_WIDTH(VALUE_WIDTH),
	.MADDR_WIDTH(MADDR_WIDTH),
	.MDATA_WIDTH(MDATA_WIDTH)
)writer
(
	reset,
	clock,
	writer_enable,
	writer_address,
	prev_vector,
	number_of_nodes,
	writer_mem_write_enable,
	mem_write_ready,
	mem_addr,
	mem_write_data,
	writer_ready,
	wait_request
);

// initial begin
// 	state = FINAL_STATE;
// end

assign isFINAL_STATe = state == FINAL_STATE;

always_comb begin : state_machine
	next_state = state;
	if (reset)
		next_state = RESET_STATE;
	else begin
		case(state)
		RESET_STATE:
			if(enable)
				next_state = READY_STATE;
		READY_STATE:
			if(enable && min_ready)
				next_state = V0;
		V0:
			if(number_of_unvisited_nodes >= 0 && current_node != destination)
				next_state = V1;
			else
				next_state = WRITE_STATE;
		V1:	
			next_state = V2;
		V2:
			if(ec_ready && min_ready)
				next_state = V3;
		V3:
			if(ec_ready)
				next_state = V4;
			else
				next_state = V2;
		V4:
			if(min_ready)
				next_state = V5;
		V5:
			//next_state = V55;
			if(pq_index >= number_of_nodes)
				next_state = V0;
			else
				next_state = V2;
		WRITE_STATE:
			if(writer_ready)
				next_state = FINAL_STATE;
		FINAL_STATE:
			next_state = FINAL_STATE;
		endcase
	end
	// case(state)
	// FINAL_STATE:
	// 	begin
	// 		mem_read_enable = 1'bz;
	// 		mem_write_enable = 1'bz;
	// 	end
	// default:
	// 	begin
		mem_read_enable = ec_mem_read_enable;
		mem_write_enable = 1'bz;//writer_mem_write_enable; //if change this to 1'bz, it works
	// 	end
	// endcase
end


always @(posedge clock)
begin
	state <= next_state;
	case(next_state)
		// Reset components
		RESET_STATE:
		begin
			ready <= 0;
			pq_index <= source;
			// All enables to zero
			pq_set_distance <= 1'b0;
			ec_query <= 1'b0;
			// Set everything as unvisited
			number_of_unvisited_nodes <= number_of_nodes;
			for(i=0;i<MAX_NODES;i=i+1)
			begin
				visited_vector[i] <= `UNVISITED;
				prev_vector[i] <= `NO_PREVIOUS_NODE;
			end
			controlled_pq_reset <= 1;
			controlled_ec_reset <= 1;
			writer_enable <= 0;
			current_node <= source;
		end
		// Wait for us to be enabled
		READY_STATE:
		begin
			controlled_pq_reset <= 0;
			controlled_ec_reset <= 0;

		end

		// New node to be visited
		V0:
		begin
			current_node <= min_distance_node_index;
			current_node_value <= min_distance_node_value;
			number_of_unvisited_nodes <= number_of_unvisited_nodes - 1;
			ec_query <= 1;
			ec_from_node <= min_distance_node_index;
			ec_to_node <= 0;
		end
		V1:
		begin
			pq_index <= 0;
			// Mark as visited
			visit_vector_true <= 1;
			visited_vector[current_node] <= `VISITED;

		end
		V2:
		begin
			visit_vector_true <= 0;
			//wait for edge cache to be ready
		end
		V3:
		begin
			if(ec_ready)
			begin
				// Check if we need to reduce
				if(current_node_value + ec_edge_value < pq_distance_read)
				begin
					// Reduce
					prev_vector[pq_index] <= current_node;
					pq_distance_to_set <= current_node_value + ec_edge_value;
					pq_set_distance <= 1;
				end
			end
		end
		V4:
		begin
			pq_set_distance<=0;
		end
		
		V5:
		begin
			pq_index <= pq_index + 1;
			ec_to_node <= ec_to_node + 1;
		end

		WRITE_STATE:
		begin
			ec_query <= 0;
			writer_enable <= 1;
			writer_address <= base_address + (number_of_nodes**2)*MADDR_WIDTH/8;
		end
		FINAL_STATE:
		begin
			ready <= 1;
			writer_enable <= 0;
			
		end
	endcase
end


endmodule
