module data_switch_send(FiftyMHz_ref_clck, Ten_MHz_wire, sending_trigger_in, switches_in, ten_MHz_synch, 
serial_data_out_1_bit, dflag);
	
//input wires:
	input Ten_MHz_wire;       	//clock signal from "clock_maker"
	input sending_trigger_in;		//trigger input from "trigger_program"
	input [11:0] switches_in;	//switch input 12-bit number
	input FiftyMHz_ref_clck; //internal clock
		
//output wires:
	output reg serial_data_out_1_bit;		//serial out line for number from switches BigEndian
	output reg dflag; 				//out line for flag to tell if data is coming
	output reg ten_MHz_synch;	//output of 10MHz signal
	
	//assign ten_MHz_synch = Ten_MHz_wire;


//temp regs:	
			//reg dflag;
	//used for counting
	reg trigger_arm;  	//tells counter when to respond to trigger and when to reset
	reg [22:0] counter; // register used to count up to 10011000100101101000000 == 5,000,000 == .5 sec
	
	
	//used for sending data
	reg [11:0] data_reg_12_bits;		//always keeps number in switches (LittleEndian)
	reg [11:0] data_output_reg_12_bits; //holds number at critical time before sending 
										//and is passed into "serial_data_out_1_bit" 1 bit at a time
	
	integer M;							//used for designating position in "data_output_reg_12_bits"
	reg trigger_flag;					//let's the program know when all 12 bits have been sent
	
	reg [2:0] clock_counter;  //register to count duty cycle and period of clock
	
	initial
		begin
		//dflag <= 1;
		end
	
	//when trigger is detected at the same time arm is off counter will start and arm will be on
	//
	//when counter is above the time at which everything is done, block will be bypassed
	//as long as the trigger is low, arm and counter will be reset to wait for another trigger
	/*1.2ms =10111011100000, .8ms = 01111101000000, .9ms = 10001100101000, 1ms = 10011100010000, 1.1ms = 10101011111000*/
	always@ (posedge Ten_MHz_wire)
		begin
		if (((sending_trigger_in == 1) && (trigger_arm == 0)) || ((counter <= 23'b10011000100101101000000) && (trigger_arm == 1))) // counter less than .5 s
			begin
			counter <= counter + 23'b000000000000000000000001;
			trigger_arm <= 1;
			
			end
			
		else if (trigger_arm == 0)
			begin
			counter <= 23'b00000000000000000000000;
			end
		else /*if (trigger == 0) and  counter > .9ms*/
			begin
			trigger_arm <= 0;
			end
		end
		
		
		
		
		
	///data sender	
	always@ (negedge Ten_MHz_wire)
		begin
		//after trigger and .8ms has passed, sends out flag signal, sends swith data to output reg
		//and sets up flag to send data
		if ((counter >= 23'b01111101000000) && (counter < 23'b10001100101000)) //counter btwn .8ms and .9 ms
			begin
			data_output_reg_12_bits <= data_reg_12_bits;
			trigger_flag <= 1;
			end
		//after 1 ms after the trigger the data is transfered to the pin connected to data_output
		//then after all 12 bits this block is bypass and signal is set low
		if ((23'b10011100010000 <= counter) && (trigger_flag == 1))// counter greater than or equal to 1ms
			begin
			dflag <= 1;
			serial_data_out_1_bit <= data_output_reg_12_bits[M];
				if (M <= 11)
					begin
					M <= M + 1;
					end
				else
					begin
					M <= 0;
					dflag <= 0;
					trigger_flag <= 0;
					serial_data_out_1_bit <= 0;
					end
			end
		
		//after .1ms a .1ms flag signal is send to mark end of transmission		
		/*10+12 for testbtwn 29-30*/	
		/*if ((23'b10101011111000 <= counter) && ( counter < 23'b0000000000011110)) //counter btwn 1.1ms and 1.2ms
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
		data_reg_12_bits <= switches_in;
		
		end
		
		
	always@ (posedge FiftyMHz_ref_clck)
		begin
		clock_counter <= clock_counter + 3'b001;
		
		if (clock_counter >= 3'b010)//if
			begin
			ten_MHz_synch <= 1'b0;
			end
		else 
			begin
			ten_MHz_synch <= 1'b1;
			end
		if (clock_counter >= 3'b100)
			begin
			clock_counter <= 3'b000;
			end
			
		end
		
endmodule

			
			
			
	



	
	
		
	
		
		
			
		
	
	