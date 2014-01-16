module data_program(Ten_MHz_input, switches_in, trigger, data_out_1_bit, dflag);
	
//input wires:
	input Ten_MHz_input;       	//clock signa' from "clock_maker"
	input [17:0] switches_in;	//switch input 18-bit number
	input trigger;				//trigger input from "safety_trigger"
		
//output wires:
	output reg data_out_1_bit;		//serial out line for number from switches BigEndian
	output reg dflag; 				//out line for flag to tell if data is coming


//temp regs:	
			//reg dflag;
	//used for counting
	reg trigger_arm;  	//tells counter when to respond to trigger and when to reset
	reg [22:0] counter; // register used to count up to 1.2ms = 12000*100ns = 10111011100000
	
	
	//used for sending data
	reg [17:0] data_reg_18_bits;		//always keeps number in switches (LittleEndian)
	reg [17:0] data_output_reg_18_bits; //holds number at critical time before sending 
										//and is passed into "data_out_1_bit" 1 bit at a time
	
	integer M;							//used for designating position in "data_output_reg_18_bits"
	reg trigger_flag;					//let's the program know when all 18 bits have been sent
	
	initial
		begin
		//dflag <= 1;
		end
	
	//when trigger is detected at the same time arm is off counter will start and arm will be on
	//10011000100101101000000 == 5,000,000 == .5 sec
	//when counter is above the time at which everything is done, block will be bypassed
	//as long as the trigger is low, arm and counter will be reset to wait for another trigger
	/*1.2ms =10111011100000, .8ms = 01111101000000, .9ms = 10001100101000, 1ms = 10011100010000, 1.1ms = 10101011111000*/
	always@ (posedge Ten_MHz_input)
		begin
		if (((trigger == 1) && (trigger_arm == 0)) || ((counter <= 23'b10011000100101101000000) && (trigger_arm == 1))) // counter less than 1.2ms
			begin
			counter <= counter + 14'b00000000000001;
			trigger_arm <= 1;
			
			end
			
		else if (trigger_arm == 0)
			begin
			counter <= 14'b00000000000000;
			end
		else /*if (trigger == 0) and  counter > .9ms*/
			begin
			trigger_arm <= 0;
			end
		end
		
		
		
		
		
	///data sender	
	always@ (negedge Ten_MHz_input)
		begin
		//after trigger and .8ms has passed, sends out flag signal, sends swith data to output reg
		//and sets up flag to send data
		if ((counter >= 14'b01111101000000) && (counter < 14'b10001100101000)) //counter btwn .8ms and .9 ms
			begin
			data_output_reg_18_bits <= data_reg_18_bits;
			trigger_flag <= 1;
			end
		//after 1 ms after the trigger the data is transfered to the pin connected to data_output
		//then after all 18 bits this block is bypass and signal is set low
		if ((14'b10011100010000 <= counter) && (trigger_flag == 1))// counter greater than or equal to 1ms
			begin
			dflag <= 1;
			data_out_1_bit <= data_output_reg_18_bits[M];
				if (M <= 17)
					begin
					M <= M + 1;
					end
				else
					begin
					M <= 0;
					dflag <= 0;
					trigger_flag <= 0;
					data_out_1_bit <= 0;
					end
			end
		
		//after .1ms a .1ms flag signal is send to mark end of transmission		
		/*10+18 for testbtwn 29-30*/	
		/*if ((14'b10101011111000 <= counter) && ( counter < 14'b0000000000011110)) //counter btwn 1.1ms and 1.2ms
			begin
			dflag <= 0;
			M <= 0;
			end*/
		
		//dflag stays low unless the message is about to be sent or ends	
		else
			begin
			dflag <= 0;
			end
			
		end
			
	
	
//always loop keeps switches parallel number in "data_reg"		
	always
		begin
		data_reg_18_bits <= switches_in;	
		end
		
endmodule

			
			
			
	



	
	
		
	
		
		
			
		
	
	