module euclid_top
#(
	parameter ADDRESS_WIDTH=4,
	parameter VECTOR_SIZE=2
)
(
	// COMMON
	input logic clk,
	input logic rst,
	// AVALON-MM SLAVE
	input logic [ADDRESS_WIDTH-1:0] slave_address,
	input logic  slave_read,
	output logic [31:0] slave_readdata,
	input logic slave_write,
	input logic [31:0] slave_writedata,
	output logic slave_waitrequest,
	// INTERRUPT SENDER
	output logic done_irq
);

reg [31:0] FILE [0:(2 ** ADDRESS_WIDTH)-1];
logic [31:0] out [0:VECTOR_SIZE-1];
logic [7:0] counter;
integer j;


assign done_irq = FILE[0][1];



// **** MEMORY READ ****
always_comb 
begin
	slave_waitrequest = 0;

	// ASYNCHRONOUS READ
	slave_readdata = (slave_read) ? FILE[slave_address] : 32'hFFFFFFFF;
end

// **** MEMORY WRITE ****
always_ff @(posedge clk) 
begin
	// SYNCHRONOUS WRITE
	if (slave_write)
		FILE[slave_address] <= slave_writedata; 
	else 
	begin
		// STATE MACHINE
		case (FILE[0][1:0])
			2'b00: //IDLE
			begin
				// Transition out of this state induced by writing to CTRL register
			end
			2'b01: //ACTIVE
			begin
				if (counter == 8'hC)
					FILE[0][1:0] <= 2'b10;
			end
			2'b10: //DONE
			begin
				// Transition out of this state induced by writing to CTRL register
			end
			default: FILE[0][1:0] <= 2'b00;
		endcase

		// COUNTER
		case (FILE[0][1:0])
			2'b01: counter <= counter + 1;
			2'b10: counter <= counter;
			default: counter <= 8'h0;
		endcase
		
		// RESULTS
		case (FILE[0][1:0])
			2'b10: for (j=0; j < VECTOR_SIZE; j = j + 1) FILE[(VECTOR_SIZE*4) + 1 + j] <= out[j];
			2'b00: for (j=0; j < VECTOR_SIZE; j = j + 1) FILE[(VECTOR_SIZE*4) + 1 + j] <= FILE[(VECTOR_SIZE*4) + 1 + j];
			default: for (j=0; j < VECTOR_SIZE; j = j + 1) FILE[(VECTOR_SIZE*4) + 1 + j] <= 32'hFFFFFFFF;
		endcase
	end
end

// instantiate the euclid_dist module and store the result in memory
// right now can only do two euclid distances obviously can increase this 
// 1) has x1 in mem[0], x2 in mem[1], y1 in mem[2], y2 in mem[3], result in mem[4]
// 2) has x1 in mem[5], x2 in mem[6], y1 in mem[7], y2 in mem[8], result in mem[9]
generate
	genvar i;
	for(i = 0; i < VECTOR_SIZE; i = i + 1) 
	begin : euclid_dist_inst
		euclid_dist euclid_dist_module (
			.reset(rst),
			.clk(clk),
			.x1(FILE[(i*4)+1]),
			.x2(FILE[(i*4)+2]),
			.y1(FILE[(i*4)+3]),
			.y2(FILE[(i*4)+4]),
			.res(out[i])
		);
	end
endgenerate
	 

endmodule
