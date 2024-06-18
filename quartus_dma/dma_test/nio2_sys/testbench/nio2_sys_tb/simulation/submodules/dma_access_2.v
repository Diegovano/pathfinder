module dma_access_2 (
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
    reg [31:0] mem [0:4];               // 5 memory locations
    reg        addition_start;          // flag to indicate we can start the addition 
    reg        readdatavalid;           // flag to indicate whatever we read is valid

    // wires
    wire        addition_done;          // flag to indicate the addition is done
    wire [31:0] addition_result;        // the result of the addition
    wire [31:0] fpadddataa, fpadddatab; // the data to be added
    wire [ 7:0] fpaddn;                 // code for the custom instruction to indicate addition
    wire        fpaddreset;             // reset signal for the addition module

    // fsm
    parameter IDLE = 2'b00, READ = 2'b01, WRITE = 2'b10, WAIT_FOR_ADDITION = 2'b11; // 4 states
    reg [ 1:0] state;                                                               // current state
    reg [ 1:0] next_state;                                                          // next state

    // assign state to next state 
    always @(posedge clk or posedge reset) begin
        if (reset) begin
            state <= IDLE;           // reset to idle state
        end else begin
            state <= next_state;     // assign the next state to the current state
        end
    end

    // fsm logic
    always @(*) begin

        // default values
        next_state         = state;  // default to current state
        readdatavalid      = 1'b0;   // default to invalid data

        case(state)
            IDLE: begin
                // if we have a read request, go to read state
                if(slaveread) begin
                    next_state <= READ;
                end
                // if we have a write request, go to write state
                else if(slavewrite) begin
                    next_state <= WRITE;
                end
            end 
            READ: begin
                // if we have a read request for address 3 and the addition has not completed, 
                // go to wait for addition state
                readdatavalid <= 1'b1;
                case (slaveaddress)
                    3'h0: slavereaddata <= mem[0];
                    3'h1: slavereaddata <= mem[1];
                    3'h2: slavereaddata <= mem[2];
                    3'h3: slavereaddata <= mem[3];
                    3'h4: begin
                            // problem for now since if we try access 4 before populating 
                            // the others then we will be stuck in wait state
                            if(!addition_done) begin
                                next_state <= WAIT_FOR_ADDITION;
                                readdatavalid <= 1'b0;
                            end else begin
                                slavereaddata <= mem[4];
                                readdatavalid <= 1'b1;
                            end
                          end
                    default: slavereaddata <= 32'b0;
                endcase
            end
            WRITE: begin
                case (slaveaddress)
                    3'h0: mem[0] <= slavewritedata;
                    3'h1: mem[1] <= slavewritedata;
                    3'h2: mem[2] <= slavewritedata;
                    3'h3: begin
                        mem[3] <= slavewritedata;
                        addition_start <= 1'b1;
                        next_state <= WAIT_FOR_ADDITION;
                    end
                    default: ;
                endcase
            end  
            WAIT_FOR_ADDITION: begin
                // if the addition is done, assign to read bus 
                if(addition_done) begin
                    slavereaddata <= mem[4];
                    readdatavalid <= 1'b1;
                    next_state <= IDLE;
                end else begin
                    readdatavalid <= 1'b0;
                    next_state <= WAIT_FOR_ADDITION;
                end
            end
            default: next_state <= IDLE;
        endcase
    end

    // assign the addition signals
    assign fpadddataa = mem[0];
    assign fpadddatab = mem[1];
    assign fpaddn      = 8'd253;  
    assign fpaddreset  = reset;

    // instantiate the fp_add module and do the addition
    fpoint_wrapper fpadd(
                .clk(clk),
                .clk_en(addition_start),
                .dataa(fpadddataa),
                .datab(fpadddatab),
                .n(fpaddn),
                .reset(fpaddreset),
                .start(addition_start),
                .done(addition_done),
                .result(addition_result));

    // assign the result to the memory
    always @(posedge clk) begin
        mem[4] <= addition_result;
    end

    assign slavewaitrequest   = 1'b0;
    assign slavereaddatavalid = readdatavalid;

endmodule 