module read_rabbit_184(key_0, fifty_MHz_int, SCLK_PE_3, SDIO_PE_5, starter, ender, LEDS, HEXX, trigger_out);

	input key_0;  //key for trigger
	input fifty_MHz_int;  //clock for trigger
	input SCLK_PE_3;  
	input SDIO_PE_5;
	input [4:0] starter;
	input [4:0] ender;
	
	output [0:7] LEDS;  //number to LEDS binary form orig freq <=255
	output trigger_out;  //trigger for rabbit
	output [55:0] HEXX;
	
	wire [0:183] full_184_bit;
	
	trigger triggering(key_0, fifty_MHz_int, trigger_out);
	reader reading(SCLK_PE_3, SDIO_PE_5, full_184_bit);
	converter converting(full_184_bit, fifty_MHz_int, starter, ender, LEDS, HEXX);
	
	
endmodule
