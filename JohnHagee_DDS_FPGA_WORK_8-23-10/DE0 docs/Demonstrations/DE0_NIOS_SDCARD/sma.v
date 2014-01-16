// sma.v

// This file was auto-generated as part of a SOPC Builder generate operation.
// If you edit it your changes will probably be lost.

module sma (
		input  wire [1:0]  s_address_in,   //       slave.address
		input  wire        s_read_in,      //            .read
		output wire [31:0] s_readdata_out, //            .readdata
		input  wire        s_write_in,     //            .write
		input  wire [31:0] s_writedata_in, //            .writedata
		input  wire        CLK_1,          // conduit_end.export
		input  wire        CLK_2,          //            .export
		input  wire        s_clk_in,       //  clock_sink.clk
		input  wire        s_reset_in      //            .reset
	);

	TERASIC_CLOCK_COUNT sma (
		.s_address_in   (s_address_in),   //       slave.address
		.s_read_in      (s_read_in),      //            .read
		.s_readdata_out (s_readdata_out), //            .readdata
		.s_write_in     (s_write_in),     //            .write
		.s_writedata_in (s_writedata_in), //            .writedata
		.CLK_1          (CLK_1),          // conduit_end.export
		.CLK_2          (CLK_2),          //            .export
		.s_clk_in       (s_clk_in),       //  clock_sink.clk
		.s_reset_in     (s_reset_in)      //            .reset
	);

endmodule
