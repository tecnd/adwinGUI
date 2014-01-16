`define PAT_NONE		0
`define PAT_H_SCALE 	1
`define PAT_V_SCALE		2

module PAT_640x480_GEN(
	pclk,  // 25M
	hs,
	vs,
	de,
	r,
	g,
	b,
	id,
);

input 			pclk;
input	[1:0]	id;
output			hs;
output			vs;
output			de;
output	[7:0]	r;
output	[7:0]	g;
output	[7:0]	b;

assign hs = 1'b1;
assign vs = 1'b1;
assign r = 0;
assign g = 0;
assign b = 0;

endmodule
