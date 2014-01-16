/*//this is the top level module for a program that will receive an 18-bit number 
//from another DE2 and print the number on its HEX display
//

module another_DE2_to_one(FiftyMHz_int_ref_clock, switch0, switch1, switch2, 
	switch3, switch4, switch5, switch6, switch7, switch8, switch9, 
	switch10, switch11, switch12, switch13, switch14, switch15, switch16, switch17,
	trigger, TenMHz_output_clock, data, flag, switch_reg_data);
	
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
	switch10, switch11, switch12, switch13, switch14, switch15, switch16, switch17, data, flag, switch_reg_data);
	
	
	
	
endmodule
*/

module one_DE2_to_another (FiftyMHz_int_ref_clock, key, switches, input_trigger, Ten_MHz_input_clock, 
input_data_1_bit, data_ctrl_input, output_trigger, TenMHz_output_clock, output_data_1_bit, data_ctrl_output/*,
output_HexDisplay*/);	// top-level module


	
//input wires
	
	//module "safety_trigger," "clock_maker"
	input FiftyMHz_int_ref_clock; //PIN_N2  internal ref clock to drive the whole system
	
	//module "safety_trigger"
	input key; //KEY[0]	PIN_G26 starts trigger

	//module "data_program"
	input [17:0] switches;// pin assignments sw[0-17] will give the system the number to transfer
	
	//module "data_program"
	input input_trigger; //gives signal to send info of the number
	
	//module "data_program," "read_data"
	input Ten_MHz_input_clock;	//input clock coming out "clock_maker" module (10MHz)  PIN_T23 GPIO1 16
	
	//module "read_data"
	input input_data_1_bit;		// input wire data input, the signal carrying the number  PIN_W25 GPIO1 34
	
	//module "read_data"
	input data_ctrl_input;   //input wire  signal telling the DE2 to start reading
	
	
	
//output wires
	
	//module "saftey_trigger"
	output output_trigger; //trigger out
	
	//module "clock_maker"
	output TenMHz_output_clock; //10 MHz clock output from " clock_maker"  module GPIO_0 9 PIN_F26
	
	//module "data_program"
	output output_data_1_bit;      //data output from data program GPIO_1 34 PIN_K25
	
	//module "data_program"
	output data_ctrl_output;   //output that corresponds with the data being sent
	
	//module "DE2_HexDisplay"
	/*output [41:0] output_HexDisplay;	// output wires from DE2_hexDisplay to actual Hex display*/
	
	
	
	
	
//wires:
	
	//module O: "data_read"  I: "DE2_HexDisplay"
	/*wire [17:0] data_for_display; //wire connecting the module reading the data to the module
								  //creating the display*/
								
	
	
	
//module calls and instances:
	
								

	//module to take send trigger when key0 is depressed
		safety_trigger trigger_shoot(FiftyMHz_int_ref_clock, key, output_trigger);
	
	

	//module to create ten MHz clock from fifty MHz clock
		clock_maker clock_outputting(FiftyMHz_int_ref_clock, TenMHz_output_clock);
	
	//module to take switches ad turn them into serial data and send them once trigger is detected
		//it uses TENMHz clock
		data_program sending_out_the_data(Ten_MHz_input_clock, switches, input_trigger, 
		output_data_1_bit, data_ctrl_output);
	
	//module to read data and turn in to parallel data for hex display using 18 wire cable "data_for_display"
		read_data data_been_read(Ten_MHz_input_clock, input_data_1_bit, data_ctrl_input, /*switches,*/ /*data_for_display*/);	// call the "read_data" module
															// with instance "data_been_read"
															
	//module to turn parallal data read from serial data by "read_data" into hex display
		/*DE2_HexDisplay Hex_will_display(data_for_display, output_HexDisplay);	// call the "DE2_HexDisplay" module
															// with instance " Hex_will_display"*/

endmodule
