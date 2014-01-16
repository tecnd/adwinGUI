module another_DE2_to_one(FiftyMHz_int_ref_clock, switch0, switch1, switch2, 
	switch3, switch4, switch5, switch6, switch7, switch8, switch9, 
	switch10, switch11, switch12, switch13, switch14, switch15, switch16, switch17,
	trigger, TenMHz_output_clock, data, flag/*, switch_reg_data*/);
	
	input FiftyMHz_int_ref_clock;//PIN_N2
	
	input switch0;
	input switch1;
	input switch2;
	input switch3;
	input switch4;
	input switch5;
	input switch6;
	input switch7;
	input switch8;
	input switch9;
	input switch10;
	input switch11;
	input switch12;
	input switch13;
	input switch14;
	input switch15;
	input switch16;
	input switch17;
	
	
	input trigger; //GPIO_1 34
	
	output TenMHz_output_clock; //GPIO_0 9 PIN_F26
	output data;      //GPIO_1 34 PIN_K25
	output flag;
	//output [17:0] switch_reg_data;
	
	
	wire TenMHz_to_data;
	
	assign TenMHz_to_data = TenMHz_output_clock;
	
	
	

	clock_maker clock_outputting(FiftyMHz_int_ref_clock, TenMHz_output_clock);
	//need to use TenMHz_TO_DATA for every clock in DATA_MAKER
	data_program sending_out_the_data(TenMHz_to_data, trigger, switch0, switch1, switch2, 
	switch3, switch4, switch5, switch6, switch7, switch8, switch9, 
	switch10, switch11, switch12, switch13, switch14, switch15, switch16, switch17, data, flag/*, switch_reg_data*/);
	
	
	
	
endmodule
