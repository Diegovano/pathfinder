module direct_dijkstra
(
  input logic av_clk,
  input logic clk,
  input logic clk_en,
  input logic av_reset,
  input logic reset,
  input logic [31:0] dataa,
  input logic [31:0] datab,
  input logic start,

  input logic write,
  input logic [31:0] writedata,

  output logic [31:0] result,
  output logic done,
  output logic waitrequest
);

  logic started = 0;
  int i = 0;

  always_ff @(posedge clk) begin
    if (reset || av_reset) begin
      result <= 0;
      done <= 0;
      i <= 0;
      started <= 0;
      waitrequest <= 1;
    end
    else if (clk_en) begin
      waitrequest <= 0;
      if (!started && start) started = 1;
      if (started) begin
        if (write) begin
          result <= result + writedata;
          i++;
        end
        else result <= (result + datab);
        if (i >= dataa) done = 1;
      end
    end
    else waitrequest <= 1;
  end
endmodule