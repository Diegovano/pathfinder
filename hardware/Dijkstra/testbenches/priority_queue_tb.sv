`include "../hdl/constants.v"
`timescale 1ps/1ps


module PriorityQueueTestbench
#(
	parameter MAX_NODES= `DEFAULT_MAX_NODES,
	parameter INDEX_WIDTH=`DEFAULT_INDEX_WIDTH,
	parameter VALUE_WIDTH=`DEFAULT_VALUE_WIDTH
)
(
);
	integer i;
	reg reset=0;
	reg clock=0;
	reg set_en=1'bz;
	reg [INDEX_WIDTH-1:0] index;
	reg [VALUE_WIDTH-1:0] write_value;
	wire [VALUE_WIDTH-1:0] read_value;
	wire[INDEX_WIDTH-1:0] min_index;
	wire[VALUE_WIDTH-1:0] min_value;
	reg [MAX_NODES-1:0] visited_vector;
	wire min_ready;
	wire [VALUE_WIDTH-1:0] dist_vector[MAX_NODES-1:0];


	PriorityQueue #(.MAX_NODES(MAX_NODES)) pq(reset, clock, set_en, index, visited_vector, write_value, read_value, min_index, min_value, min_ready, dist_vector);

	// Setup clock to automatically strobe with a period of 20.
	always #10000 clock = ~clock;

	integer min = `INFINITY;
	initial
		begin
		// Mark all as unvisited for test purposes
		for(i=0;i<MAX_NODES;i=i+1)
		begin
			visited_vector[i] = `UNVISITED;
		end

		// Reset 
		reset = 0;
		set_en = 1'bz;
		@(posedge clock);
		@(posedge clock);
		clock = 1'b0;
		reset = 1'b1;
		index = 1'b0;
		@(posedge clock);
		@(posedge clock);
		reset = 1'b0;
		$display("Reset complete");


		// Confirm we reset correctly
		@(posedge clock);
		@(posedge clock);
		if(read_value !== 0)
		begin
			$fatal(1, "Source should be 0");
		end
		for(index=1;index<MAX_NODES;index=index+1)
		begin
			@(posedge clock);
			@(posedge clock);
			if(read_value !== `INFINITY)
			begin
				$fatal(1, "dist[%d] = %d but should be infinity", index, read_value);
			end
		end

		while(!min_ready)
			@(posedge clock);

		if(min_value !== 0)
		begin
			$fatal(1,"Min value should initially be 0");
		end

		if(min_index !== 0)
		begin
			$fatal(1, "Min index should initially be 0");
		end

		$display("Initial values are correct");


		// Set source-to-source distance to infinity
		// So we can find the minimum value later
		// (because otherwise source will always be the minimum value, being
		// zero)
		@(posedge clock);#1;
		@(posedge clock);#1;
		set_en=1;
		write_value = `INFINITY;
		index = 0;
		@(posedge clock);#1;
		@(posedge clock);#1;
		set_en = 1'bz;
		while(!min_ready)
			@(posedge clock);
		if(min_value !== `INFINITY)
		begin
			$fatal(1, "Min value should initially be infinity");
		end

		// Make sure we can read and write
		for(index=0;index<MAX_NODES;index=index+1)
		begin
			@(posedge clock);#1;
			@(posedge clock);#1;

			set_en = 1;
			write_value = $urandom % 50;
			if (min>write_value)
				min=write_value;

			@(posedge clock);#1;
			@(posedge clock);#1;
			set_en = 1'bz;

			@(posedge clock);#1;
			@(posedge clock);#1;

			if(read_value !== write_value)
			begin
				$fatal(1, "FUCK");
			end

			while(!min_ready)
				@(posedge clock);
			if(min !== min_value)
			begin
				$fatal(1, "Minimum value is incorrect. Is %d, but should be %d", min_value, min);
			end
		end

		$display("Test completed successfully");
		$finish;


	end

endmodule //
