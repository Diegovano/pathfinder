`timescale 1ps/1ps

module DijkstraTopTestbench
();
	reg reset = 0;
	reg clock = 0;
	reg enable = 0;

    reg mem_clock = 0;
    reg mem_reset = 0;

    reg [31:0] dummy_base_address = 3;
	reg [31:0] datab = 1;

    wire mem_read_enable;
	wire mem_write_enable;
    wire mem_write_ready;
	wire mem_read_ready;
    wire [1:0] mem_write_response;


	wire [31:0] mem_addr;
	wire [15:0] mem_write_data;
	wire [15:0] mem_read_data;

    wire [31:0] shortest_distance;

    reg ready = 0;
    wire wait_request = 1;

    DummyInterface dummy_interface(
        reset,
        clock,
        enable,
        mem_clock,
        mem_reset,
        dummy_base_address,
        datab,
        mem_read_enable,
        mem_write_enable,
        mem_write_ready,
        mem_read_ready,
        mem_write_response,
        mem_addr,
        mem_read_data,
        mem_write_data,
        shortest_distance,
        ready,
        wait_request
    );

    always #10000 clock = ~clock;

    initial begin
        
        $display("Starting testbench");


        
        dummy_base_address = 1;
        datab = 2;
        @(posedge clock);
        if (!ready) begin
            $display("Waiting for ready signal");
            @(posedge clock);
        end
        $display("sum 1+2: %d", shortest_distance);
        @(posedge clock);
        dummy_base_address = 2;
        datab = 3;
        @(posedge clock);
        $display("sum 2+3: %d", shortest_distance);
        @(posedge clock);


        $finish;
    end

endmodule
