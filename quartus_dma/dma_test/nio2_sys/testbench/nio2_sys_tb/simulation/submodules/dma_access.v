module dma_access (
    input  wire        reset,              //          reset.reset
    input  wire        clk,                //          clock.clk
    input  wire        slaveread,          //          avalon_slave_0.read
    output reg  [31:0] slavereaddata,      //          .readdata
    input  wire        slavewrite,         //          .write
    input  wire [31:0] slavewritedata,     //          .writedata
    output wire        slavereaddatavalid, //          .readdatavalid
    output wire        slavewaitrequest,   //          .waitrequest
    input  wire [ 2:0] slaveaddress        //          .address (assuming 2-bit address for simplicity) (3 bit for result)
);

    // internal signals 
    // registers:
    reg [31:0] ip1, ip2, ip3, ip4, result; // registers for each address
    //reg [31:0] mem [0:4];               // 5 memory locations
    reg        addition_start;          // flag to indicate we can start the addition 
    reg        readdatavalid;           // flag to indicate whatever we read is valid

    // wires
    wire        fpaddclk;               // clock signal for the addition module
    wire        addition_done;          // flag to indicate the addition is done
    wire [31:0] addition_result;        // the result of the addition
    wire [31:0] fpadddataa, fpadddatab; // the data to be added
    wire [ 7:0] fpaddn;                 // code for the custom instruction to indicate addition
    wire        fpaddreset;             // reset signal for the addition module
    wire        fpaddstart;             // start signal for the addition module
   
    // logic for writing the data to registers
    always @(posedge clk or posedge reset) begin
        if (reset) begin
            // if reset, then reset all the registers
            ip1 <= 32'h0;
            ip2 <= 32'h0;
            ip3 <= 32'h0;
            ip4 <= 32'h0;
            result <= 32'h0;
            addition_start <= 1'b0;
        end else begin
            // if write request, then write the data to the registers depending on the address
            if (slavewrite) begin
                case (slaveaddress)
                    3'h0: ip1 <= slavewritedata;
                    3'h1: ip2 <= slavewritedata;
                    3'h2: ip3 <= slavewritedata;
                    3'h3: begin
                        // if writing to the 4th address, then start the addition
                        ip4 <= slavewritedata;
                        addition_start <= 1'b1;
                    end
                    default: ;
                endcase
            end else begin
                // if no write request, then reset the addition start flag
                addition_start <= 1'b0;
            end
        end
    end

    // assign the addition signals
    assign fpaddclk    = clk;
    assign fpadddataa  = ip3;
    assign fpadddatab  = ip4;  
    assign fpaddn      = 8'd253;  
    assign fpaddreset  = reset;
    assign fpaddstart  = addition_start;
    
    assign slavewaitrequest = 1'b0;

    // logic for reading the data
    always @(posedge clk or posedge reset) begin
        if (reset) begin
            // if reset, then read data is invalid and output 0
            readdatavalid <= 1'b0;
            slavereaddata <= 32'h0;
        end else if (slaveread) begin
            // if read request, then output the data depending on the address
            readdatavalid <= 1'b1;
            case (slaveaddress)
                3'h0: slavereaddata <= ip1;
                3'h1: slavereaddata <= ip2;
                3'h2: slavereaddata <= ip3;
                3'h3: slavereaddata <= ip4;
                3'h4: begin
                        if (addition_done) begin
                            // if addition is done, then output the result
                            slavereaddata <= result;
                        end else begin
                            // if addition is not done, then output 0
                            readdatavalid <= 1'b0;
                        end
                      end
                default: slavereaddata <= 32'h0;
            endcase
        end else begin
            // if no read request, then read data is invalid and output 0
            readdatavalid <= 1'b0;
            slavereaddata <= 32'h0;
        end
    end

    // instantiate the fp_add module and do the addition
    fpoint_wrapper fpadd(
                .clk(fpaddclk),
                .clk_en(fpaddstart),
                .dataa(fpadddataa),
                .datab(fpadddatab),
                .n(fpaddn),
                .reset(fpaddreset),
                .start(fpaddstart),
                .done(addition_done),
                .result(addition_result));

    // assign the valid signal to the internal valid register
    assign slavereaddatavalid = readdatavalid;

endmodule
