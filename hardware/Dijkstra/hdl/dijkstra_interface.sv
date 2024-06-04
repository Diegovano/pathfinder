
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
	parameter MAX_NODES=`DEFAULT_MAX_NODES,
	parameter INDEX_WIDTH=`DEFAULT_INDEX_WIDTH,//nios system accepts 32 bit as input datab,
                                // which we are using to enter three parameters, source, destination and number of nodes
                                // this is a temporary solution, we will change those to memory access later
                                // or later extend the algorithm to calculate the shortest path between all nodes
    parameter VALUE_WIDTH=`DEFAULT_VALUE_WIDTH,
    parameter MADDR_WIDTH=`DEFAULT_MADDR_WIDTH,
    parameter MDATA_WIDTH=`DEFAULT_MDATA_WIDTH
)
(
    //input wire main_reset, // reset signal for the whole system, currently only a placeholder
	input wire algorithm_reset,
	input wire algorithm_start,
	input wire algorithm_clock,
	input wire algorithm_enable,

	input wire mem_clock, //these are not actually used, but required for the interface
	input wire mem_reset,

    input wire[MADDR_WIDTH-1:0] base_address, // dataa
    input wire[31:0] datab, // datab, 
        // datab[9:0] = source
        // datab[19:10] = destination
        // datab[29:20] = number of nodes
        // datab[31:30] = 0

	// input wire[INDEX_WIDTH-1:0] source,
	// input wire[INDEX_WIDTH-1:0] destination,
	// input wire[INDEX_WIDTH-1:0] number_of_nodes,

	

	output wire mem_read_enable,
	output wire mem_write_enable,

	input wire mem_write_ready,
	input wire mem_read_ready,
    input wire [1:0] mem_write_response, // required for interface, not actually used

	output wire [MADDR_WIDTH-1:0] mem_addr,
	input wire [MDATA_WIDTH-1:0] mem_read_data,
	output wire [MDATA_WIDTH-1:0] mem_write_data,

	output wire[31:0] shortest_distance,

	output reg ready,

	input wire wait_request
);

reg mem_clock_sink;
reg mem_reset_sink;

assign mem_clock_sink = mem_clock;
assign mem_reset_sink = mem_reset;

reg wait_request_sink;
assign wait_request_sink = 0;

// assign mem_read_enable = 1'bz;
//assign mem_write_enable = 1'bz;
// assign mem_addr = 32'bz;
wire isFINAL_STATE;

DijkstraTop #(.MAX_NODES(MAX_NODES), .INDEX_WIDTH(INDEX_WIDTH), 
    .VALUE_WIDTH(VALUE_WIDTH), .MADDR_WIDTH(MADDR_WIDTH), .MDATA_WIDTH(MDATA_WIDTH))
    dijkstra(
		algorithm_start, //reset
		algorithm_clock,
		algorithm_enable,
		datab[9:0], // source
		datab[19:10], // destination
        datab[29:20], // number of nodes
		base_address,
		mem_read_enable,
		mem_write_enable,
		mem_write_ready,
		mem_read_ready,
		mem_addr,
		mem_read_data,
		mem_write_data,
		shortest_distance,
		ready,
		wait_request_sink,
		isFINAL_STATE
	);


// assign LEDG[6] = 1;
// assign LEDG[7] = isFINAL_STATE;
endmodule