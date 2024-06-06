module lef
(
  input logic[31:0] dataa;
  input logic[31:0] datab;
  output logic[31:0] result;
)

logic signa = dataa[31];
logic signb = datab[31];
logic [7:0] expa = dataa[30:23];
logic [7:0] expb = datab[30:23];
logic [22:0] manta = dataa[22:0];
logic [22:0] mantb = datab[22:0];

if (signa > signb) assign result = 32'b1; // neg leq pos
else if (signa < signb) assign result = 32'b0; // pos leq neg
else if (signa == 0) begin // signa and signb = 0 (both pos)
  if (expa < expb) assign result = 32'b1;
  else if (expb > expa) assign result = 32'b0;
  else begin

  end
end
else begin // signa and signb = 1
  if (expa < expb) assign result = 32'b0;
  else if (expa > expb) assign result = 32'b1;
end

endmodule