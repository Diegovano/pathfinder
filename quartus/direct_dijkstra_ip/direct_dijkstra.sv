module direct_dijkstra
(
  input logic clk,
  input logic reset,
  input logic [31:0] dataa,
  input logic [31:0] datab,
  // input logic [7:0] address,
  // input logic read,
  input logic write,
  input logic [31:0] writedata,
  // output logic waitrequest,
  output logic [31:0] result //,
  // output logic [31:0] readdata
);
  // logic [31:0] nonZero;

  // initial begin
  //   // nonZero = 0;
  //   result = 0;
  // end

  // always_comb begin
  //   // result = nonZero;
  //   // result = writedata;
  // end

  // always_ff @( posedge clk ) begin
  //   // nonZero <= (write && writedata == 0) ? nonZero : writedata;
  //   result <= write ? writedata : result;
  // end

  assign result = dataa + datab + writedata;

endmodule