module dijkstra_check_step
  (
    input logic[31:0] dataa,
    input logic[31:0] datab,
    input logic clk,
    input logic clk_en,
    input logic reset,
    input logic start,
    output logic done,
    output logic[31:0] result
  );

  logic[31:0] add_res;
  int count;

  fp_add adder
  (
    .clk(clk),
    .areset(reset),
    .a(dataa),
    .b(datab),
    .q(add_res)
  );

  always_comb begin
    if (reset)
    begin
      done = 0;
      result = 0;
    end
    // else if (clk_en && dataa == 32'h7F800000 || datab == 32'h7F800000 || datab[31] == 1 || datab[30:0] == 31'b0)
    // begin
    //   result = 32'h7f800000; // infinity
    //   done = 1;
    // end
    else 
    begin
      case (count)
        2: begin
          result = (dataa == 32'h7F800000 || datab == 32'h7F800000 || datab[31] == 1 || datab[30:0] == 31'b0) ?
            32'h7f800000 : add_res;
          done = 1;
        end
        default: begin
          result = 0;
          done = 0;
        end
      endcase
    end
  end

  always @(posedge clk) begin
    if (clk_en) 
    begin
      if (reset || count > 1) count = 0;
      else if (count != 0 || start) count = count + 1;
    end
  end
endmodule