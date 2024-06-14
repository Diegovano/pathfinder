module direct_dijkstra
(
  input logic clk,
  input logic ci_clk,
  input logic ci_clk_en,
  input logic reset,
  input logic ci_reset,
  input logic [31:0] dataa,
  input logic [31:0] datab,
  input logic start,

  input logic write,
  input logic [31:0] writedata,

  output logic [31:0] result,
  output logic done
);


endmodule