`timescale 1ps/1ps

module DummyInterface_tb;

    // Parameters
    parameter MEM_DELAY = 3;
    
    // Signals
    reg algorithm_reset;
    reg algorithm_start;
    reg algorithm_clock;
    reg algorithm_enable;
    reg mem_clock;
    reg mem_reset;
    reg [31:0] base_address;
    reg [31:0] datab;
    reg mem_read_ready;
    reg wait_request;
    
    wire mem_read_enable;
    wire [31:0] mem_addr;
    wire [31:0] shortest_distance;
    wire ready;
    
    // Memory response signals
    reg [16:0] mem_read_data;
    reg [16:0] memory [0:255]; // Dummy memory
    integer i;
    
    // Instantiate the DUT (Device Under Test)
    DummyInterface DUT (
        .algorithm_reset(algorithm_reset),
        .algorithm_start(algorithm_start),
        .algorithm_clock(algorithm_clock),
        .algorithm_enable(algorithm_enable),
        .mem_clock(mem_clock),
        .mem_reset(mem_reset),
        .base_address(base_address),
        .datab(datab),
        .mem_read_enable(mem_read_enable),
        .mem_read_ready(mem_read_ready),
        .mem_addr(mem_addr),
        .mem_read_data(mem_read_data),
        .shortest_distance(shortest_distance),
        .ready(ready),
        .wait_request(wait_request)
    );
    
    // Generate clock, extreme case where different frequencies and phases
    always #5 algorithm_clock = ~algorithm_clock;
    always #7 mem_clock = ~mem_clock;
    
    // Testbench procedure
    initial begin
        // Initialize signals
        algorithm_reset = 1;
        algorithm_start = 0;
        algorithm_clock = 0;
        algorithm_enable = 0;
        mem_clock = 0;
        mem_reset = 0;
        base_address = 32'h00000010;
        datab = 32'h00000000;
        mem_read_ready = 0;
        wait_request = 0;
        mem_read_data = 0;
        
        // Initialize dummy memory
        for (i = 0; i < 256; i = i + 1) begin
            memory[i] = i;
        end
        
        // Release reset
        @ (posedge algorithm_clock);
        @ (posedge algorithm_clock);

        algorithm_reset = 0;

        for (i = 0; i < 10; i = i + 1) begin
           base_address = i;
            
            // Start algorithm
            algorithm_start = 1;
            @ (posedge algorithm_clock);
            algorithm_start = 0;
            
            // Wait for ready signal
            wait (ready);
            
            // Display result
            $display("Shortest Distance: %d", shortest_distance);
            
            // End simulation
            @(posedge algorithm_clock);
            @(posedge algorithm_clock);
        end

        @(posedge algorithm_clock);
        @(posedge algorithm_clock);
        @(posedge algorithm_clock);
        $finish;
    end
    
    
    // Memory read with delay
    reg [MEM_DELAY-1:0] read_pipeline;
    always @(posedge mem_clock or posedge mem_reset) begin
        if (mem_reset) begin
            read_pipeline <= 0;
            mem_read_ready <= 0;
        end else begin
            read_pipeline <= {read_pipeline[MEM_DELAY-2:0], mem_read_enable};
            mem_read_ready <= read_pipeline[MEM_DELAY-1];
        end
    end
    
    always @(posedge mem_clock) begin
        if (mem_read_enable) begin
            mem_read_data <= memory[{mem_addr[31:1],1'b0}];
        end
    end
    
endmodule
