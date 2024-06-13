
`timescale 1ps/1ps


module DummyInterface
(
    input wire algorithm_reset,
	input wire algorithm_start,
	input wire algorithm_clock,
	input wire algorithm_enable,

	input wire mem_clock,
	input wire mem_reset,

    input wire[31:0] base_address, // dataa
    input wire[31:0] datab, // datab, 

	output reg mem_read_enable,
	//output wire mem_write_enable,

	//input wire mem_write_ready,
	input wire mem_read_ready,
    
	//input wire [1:0] mem_write_response, // required for interface, not actually used

	output reg [31:0] mem_addr,
	input wire [15:0] mem_read_data,
	//output wire [MDATA_WIDTH-1:0] mem_write_data,

	output reg[31:0] shortest_distance,

	output reg ready, //done

	input wire wait_request
);


typedef enum logic [1:0] {IDLE,WAIT, READ, OUTPUT} state_a;
state_a algorithm_state, algorithm_next_state;


typedef enum logic [1:0] {MEM_IDLE, MEM_READ1,MEM_READ2, MEM_OUTPUT} state_m;
state_m memory_state, memory_next_state;


always @(posedge algorithm_clock, posedge algorithm_reset) begin
	if (algorithm_reset) begin
		algorithm_state <= IDLE;
	end else begin
		algorithm_state <= algorithm_next_state;
	end
end

always @(posedge mem_clock, posedge mem_reset) begin
    if (mem_reset) begin
        memory_state <= MEM_IDLE;
    end else begin
        memory_state <= memory_next_state;
    end
end

reg [15:0] mem_read_data_internal; // Internal memory read data, communicate between clock domains

// 1. Algorithm state machine
always_comb begin
    // Default assignments to avoid latches
    ready = 0;
    shortest_distance = 32'hdeadbeef;
    algorithm_next_state = algorithm_state;
    case (algorithm_state)
        IDLE: begin
            if (algorithm_start) begin
                algorithm_next_state = WAIT;
            end
        end
        WAIT: begin
            if (memory_state == MEM_IDLE) begin
                algorithm_next_state = READ;
            end
        end
        
        READ: begin
            if (memory_state == MEM_OUTPUT) begin
                algorithm_next_state = OUTPUT;
            end
        end
        
        OUTPUT: begin
            ready = 1;
            shortest_distance = mem_read_data_internal;
            algorithm_next_state = IDLE;
        end
        default: begin
            algorithm_next_state = IDLE;
        end
    endcase
end

// 2. Memory state machine
always_latch begin
    // Default assignments to avoid latches
    mem_addr = 32'bz;
    mem_read_enable = 1'bz;
    memory_next_state = memory_state;
    //mem_read_data_internal = 16'hbeef; //TODO: fix this
    case (memory_state)
        MEM_IDLE: begin
            if (algorithm_state == READ) begin
                memory_next_state = MEM_READ1;
            end
        end
        
        MEM_READ1: begin
            mem_addr = base_address;
            mem_read_enable = 1'b1;
            if (!wait_request) begin
                memory_next_state = MEM_READ2;
            end
        end
        MEM_READ2: begin
            if (mem_read_ready) begin
                memory_next_state = MEM_OUTPUT;
                mem_read_data_internal = mem_read_data;
            end
        end
        
        MEM_OUTPUT: begin
            if (algorithm_state == IDLE || algorithm_state == WAIT) begin
                memory_next_state = MEM_IDLE;
            end
        end
        default: begin
            memory_next_state = MEM_IDLE;
        end
    endcase
end



endmodule