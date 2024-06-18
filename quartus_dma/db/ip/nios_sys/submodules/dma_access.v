module dma_access (
input wire reset, // reset.reset
input wire clk, // clock.clk
input wire slaveread, // avalon_slave_0.read
output reg [31:0] slavereaddata, // .readdata
input wire slavewrite, // .write
input wire [31:0] slavewritedata, // .writedata
output reg slavereaddatavalid, // .readdatavalid
output wire slavewaitrequest, // .waitrequest
input wire [2:0] slaveaddress // .address (assuming 2-bit address for simplicity) (3 bit for result)
);
// registers for each address
reg [31:0] ip1, ip2, ip3, ip4, result;

// addition signals
wire fpaddclk;
wire fpaddclk_en;
wire [31:0] fpadddataa;
wire [31:0] fpadddatab;
wire [ 7:0] fpaddn;
wire fpaddreset;
wire fpaddstart;
wire fpadddone;
wire [31:0] fpaddresult;

// clock enables for each module
reg add_clk_en;
// reg mult_clk_en;
// reg sub_clk_en;
// reg sqrt_clk_en;

always @(posedge clk)
    begin
        if (reset) begin
            ip1 <= 32'h0;
            ip2 <= 32'h0;
            ip3 <= 32'h0;
            ip4 <= 32'h0;
            add_clk_en <= 1'b0;
        end else begin
            if (slavewrite) begin
                case (slaveaddress)
                    3'h0: ip1 <= slavewritedata;
                    3'h1: ip2 <= slavewritedata;
                    3'h2: ip3 <= slavewritedata;
                    3'h3: begin
                        ip4 <= slavewritedata;
                        add_clk_en <= 1'b1;
                    end
                    default: ;
                endcase
            end else begin
                add_clk_en <= 1'b0;
            end
        end
    end


// assign the addition signals
assign fpaddclk    = clk;
assign fpaddclk_en = add_clk_en;
assign fpadddataa  = ip3;
assign fpadddatab  = ip4;  
assign fpaddn      = 8'd253;  
assign fpaddreset  = reset;
assign fpaddstart  = add_clk_en;
assign fpaddresult = result;


always @(posedge clk)
    begin
        if (reset) begin
            slavereaddatavalid <= 1'b0;
            slavereaddata <= 32'h0;
        end else if (slaveread) begin
            slavereaddatavalid <= 1'b1;
            case (slaveaddress)
                3'h0: slavereaddata <= ip1;
                3'h1: slavereaddata <= ip2;
                3'h2: slavereaddata <= ip3;
                3'h3: slavereaddata <= ip4;
                3'h4: slavereaddata <= result;
                default: slavereaddata <= 32'h0;
            endcase
        end else begin
            slavereaddatavalid <= 1'b0;
        end
    end


assign slavewaitrequest = 1'h0;

// do the addition
fpoint_wrapper fpadd(
                .clk(fpaddclk),
                .clk_en(fpaddclk_en),
                .dataa(fpadddataa),
                .datab(fpadddatab),
                .n(fpaddn),
                .reset(fpaddreset),
                .start(fpaddstart),
                .done(fpadddone),
                .result(fpaddresult));
endmodule