module euclid_top
#(
	parameter ADDRESS_WIDTH=4
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
	end
end

// instantiate the euclid_dist module and store the result in memory
// right now can only do two euclid distances obviously can increase this 
// 1) has x1 in mem[0], x2 in mem[1], y1 in mem[2], y2 in mem[3], result in mem[4]
// 2) has x1 in mem[5], x2 in mem[6], y1 in mem[7], y2 in mem[8], result in mem[9]
generate
	genvar i;
	for(i = 1; i < 10; i = i + 5) 
	begin : euclid_dist_inst
		logic [31:0] out;
		euclid_dist euclid_dist_module (
			.reset(reset),
			.clk(clk),
			.x1(FILE[i]),
			.x2(FILE[i+1]),
			.y1(FILE[i+2]),
			.y2(FILE[i+3]),
			.dist(out)
		);
		
		//RESULT
		always_ff@(posedge clk)
		begin
			if (!slave_write & (FILE[0][1:0] == 2'b10))
				FILE[i+4] <= out;
			else
				FILE[i+4] <= 32'hFFFFFFFF;
		end
	end
endgenerate
	 

endmodule
