module euclid_top(
    input  wire        reset,              //          reset.reset
    input  wire        clk,                //          clock.clk
    input  wire        slaveread,          //          avalon_slave_0.read
    input  wire [ 3:0] slaveaddress,       //          .address (assuming 2-bit address for simplicity) (3 bit for result)
    input  wire        slavewrite,         //          .write
    input  wire [31:0] slavewritedata,     //          .writedata
    output reg         slavereaddatavalid, //          .readdatavalid
    output wire        slavewaitrequest,   //          .waitrequest
    output reg  [31:0] slavereaddata       //          .readdata
);

    reg  [31:0] mem [0:9]; // 8 memory locations
	wire [31:0] dist[0:1]; // temp storage   

    // write to memory 
	integer j;
    always @(posedge clk or posedge reset) begin
        if(reset) begin
            for (j = 0; j < 10; j = j + 1) begin mem[j] <= 0; end
        end else begin
            if(slavewrite) begin
                mem[slaveaddress] <= slavewritedata;
            end
				mem[4] <= dist[0];
				mem[9] <= dist[1];
        end
    end
	 

    // read from memory
    always @(posedge clk or posedge reset) begin
        if(reset) begin
            slavereaddata <= 32'b0;
            slavereaddatavalid <= 1'b0;
        end else begin
            if(slaveread) begin
                slavereaddata <= mem[slaveaddress];
                slavereaddatavalid <= 1'b1;
            end else begin
                slavereaddatavalid <= 1'b0;
                slavereaddata <= 32'b0;
            end
        end
    end

    // instantiate the euclid_dist module and store the result in memory
    // right now can only do two euclid distances obviously can increase this 
    // 1) has x1 in mem[0], x2 in mem[1], y1 in mem[2], y2 in mem[3], result in mem[4]
    // 2) has x1 in mem[5], x2 in mem[6], y1 in mem[7], y2 in mem[8], result in mem[9]
    generate
        genvar i;
        for(i = 0; i < 2; i = i + 1) begin : euclid_dist_inst
            euclid_dist euclid_dist_module (
                .reset(reset),
                .clk(clk),
                .x1(mem[i*5]),
                .x2(mem[i*5 + 1]),
                .y1(mem[i*5 + 2]),
                .y2(mem[i*5 + 3]),
                .dist(dist[i])
            );
        end
    endgenerate
	 
	assign slavewaitrequest = 1'b0;


endmodule 