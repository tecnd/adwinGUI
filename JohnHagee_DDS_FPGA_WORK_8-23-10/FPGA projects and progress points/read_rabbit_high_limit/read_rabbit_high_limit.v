module read_rabbit_high_limit(key_0, fifty_MHz_int, SCLK_PE_3, SDIO_PE_5, /*LEDS,*/HEXX, trigger_out);

	input key_0;  //key for trigger
	input fifty_MHz_int;  //clock for trigger
	input SCLK_PE_3;  
	input  SDIO_PE_5;
	
	//output [0:7] LEDS;  //number to LEDS binary form orig freq <=255
	output trigger_out;  //trigger for rabbit
	output [55:0] HEXX;
	
	wire [0:31] FTW_high_32_bit;
	
	trigger triggering(key_0, fifty_MHz_int, trigger_out);
	reader reading(SCLK_PE_3, SDIO_PE_5, FTW_high_32_bit);
	converter converting(FTW_high_32_bit, /*LEDS,*/ HEXX);
	
	
endmodule

	
	
	
	