//This module creates the 10MHz clock from the 50MHz one

module clock_maker(FiftyMHz_ref_clock, TenMHz_output);
	
//input wires:
	input FiftyMHz_ref_clock; //internal clock
	
//output wires:
	output reg TenMHz_output;	//output of 10MHz signal
//temp regs:	
	reg [2:0] clock_counter;  //register to count duty cycle and period of clock
	
	
//always loop creates output of 10MHz from 50MHz signal.
//duty cycle is 40%
	always@ (posedge FiftyMHz_ref_clock)
		begin
		clock_counter <= clock_counter + 3'b001;//counter values go: 001, 010, 011, 100, 101
												//					  H		H,	 L,   L,  L
		if (clock_counter >= 3'b010)//if
			begin
			TenMHz_output <= 1'b0;
			end
		else 
			begin
			TenMHz_output <= 1'b1;
			end
		if (clock_counter >= 3'b100)
			begin
			clock_counter <= 3'b000;
			end
			
		end

endmodule
