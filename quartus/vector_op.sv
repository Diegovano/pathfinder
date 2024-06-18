module vector_op
#(
	parameter ADDRESS_WIDTH=3
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
reg [7:0] counter;

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
				if (counter == 8'hFF)
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

		// RESULT
		case (FILE[0][1:0])
			2'b10: FILE[1] <= FILE[2] + FILE[3] + FILE[4] + FILE[5];
			default: FILE[1] <= 32'hFFFFFFFF;
		endcase
	end
end

endmodule
