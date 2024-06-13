
`timescale 1ps/1ps


module DummyInterface
(
    input wire algorithm_reset,
	input wire algorithm_start,
	input wire algorithm_clock,
	input wire algorithm_enable,

	input wire mem_clock, //these are not actually used, but required for the interface
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

// dummy adder
// integer countdown;
// `define COUNTDOWN_MAX 500

// always @(posedge algorithm_clock, posedge algorithm_reset) begin
// 	if (algorithm_reset) begin
// 		countdown <= 0;
// 		ready <= 0;
// 	end else begin
// 		if (algorithm_start) begin
// 			countdown <= `COUNTDOWN_MAX;
// 			ready <= 0;
// 		end else begin
// 			if (countdown > 0) begin
// 				countdown <= countdown - 1;
// 			end
// 			else begin
// 				ready <= 1;
// 			end
// 		end
// 	end	
// end

// // assign shortest_distance = base_address + datab;
// assign mem_read_enable = 1'bz;

//  reg [31:0] temp;  // Temporary register to hold intermediate results
//     integer i;        // Loop variable

//     always @(*) begin
//         temp = base_address;
//         for (i = 0; i < 1000; i = i + 1) begin
//             temp = temp + 1;
//         end
//         shortest_distance = temp;
//     end

//// dummy memory reader
typedef enum logic [1:0] {IDLE, READ, OUTPUT, FINAL} state_t;
state_t state, next_state;
always @(posedge algorithm_clock, posedge algorithm_reset) begin
	if (algorithm_reset) begin
		state <= IDLE;
	end else begin
		state <= next_state;
	end
end

always_comb begin
    // Default assignments to avoid latches
    ready = 0;
    mem_addr = 32'bz;
    mem_read_enable = 1'bz;
    shortest_distance = 354;
    next_state = state;
    case (state)
        IDLE: begin
            if (algorithm_start) begin
                next_state = READ;
            end
        end
        
        READ: begin
            mem_addr = base_address;
            mem_read_enable = 1'b1;
            // if (mem_read_ready && !wait_request) begin
            //     next_state = OUTPUT;
            // end
            if (!wait_request) begin //maintain read state until memory is ready
                next_state = OUTPUT;
            end
        end
        
        OUTPUT: begin
            if (mem_read_ready) begin //wait until data valid
                next_state = IDLE;
                shortest_distance = {16'b0, mem_read_data} + datab;
                ready = 1;
            end
        end
        FINAL: begin
            next_state = IDLE;
        end
        
        // default: begin
        //     next_state = IDLE;
        // end
    endcase
end



endmodule