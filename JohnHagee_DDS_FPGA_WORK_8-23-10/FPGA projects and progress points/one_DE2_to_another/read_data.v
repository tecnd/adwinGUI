





//This module reads the data from the external data input labeled "read_data" 

module read_data(input_clk_DE2, read_data, flag, /*switch,*/ data_read_to_display);
	
//input wires:
	input input_clk_DE2;    //10MHz clock from the external wire from other module
	input read_data;			//1-bit register to place one at a time into DATA_REG
	input flag;				//reads if this is high
	output reg [17:0] data_read_to_display;  //parallel output of serial data read in
	
	//input [17:0] switch;
	
//temp regs:
	integer N;				//integer to give place in DATA_REG
	reg [17:0] data_reg;	//18-bit register to have data read into it
	reg flagg;				//
	
	initial
		begin
		flagg = 0;
		N = 0;
				
		end
		
		
//always loop must read data in serial and send in parallel to hexdisplay
	always@ (posedge input_clk_DE2)
		begin
		if (((flag == 1) && (N <= 17)) || (flagg == 1) )
			begin
			data_reg[N] <= read_data;  //as N cycles from 0 to 17
			flagg <= 1;
			N <= N + 1;
			end
			
				
		
		if (( N >= 18) && (flagg ==1))
			begin
			data_read_to_display <= data_reg;
			N <= 0;
			flagg<= 0;
			
			
			
			end
		
		/*else
			begin
			flagg <= 0;
			end*/
			
		end
		
endmodule

	
	
	