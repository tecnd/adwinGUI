module data_program(TenMHzClock, trgger, swtch0, swtch1, swtch2, 
	swtch3, swtch4, swtch5, swtch6, swtch7, swtch8, swtch9, 
	swtch10, swtch11, swtch12, swtch13, swtch14, swtch15, swtch16, swtch17, data_output, dflag, data_reg);
	
	input TenMHzClock;
	input trgger;
	input swtch0;
	input swtch1;
	input swtch2;
	input swtch3;
	input swtch4;
	input swtch5;
	input swtch6;
	input swtch7;
	input swtch8;
	input swtch9;
	input swtch10;
	input swtch11;
	input swtch12;
	input swtch13;
	input swtch14;
	input swtch15;
	input swtch16;
	input swtch17;
	
	output data_output;
	output dflag;
	output [17:0] data_reg;
	
	reg dflag;
	reg data_output;
	reg trgger_flag;
	reg trgger_arm;
	reg [17:0] data_reg;
	reg [17:0] data_output_reg;
	integer M;
	reg [13:0] counter;
	
	//when trigger is detected at the same time arm is off counter will start and arm will be on
	//when counter is above the time at which everything is done, block will be bypassed
	//as long as the trigger is low, arm and counter will be reset to wait for another trigger
	/*1.2ms =10111011100000, .8ms = 01111101000000, .9ms = 10001100101000, 1ms = 10011100010000, 1.1ms = 10101011111000*/
	always@ (posedge TenMHzClock)
		begin
		if (((trgger == 1) && (trgger_arm == 0)) || ((counter <= 14'b00000000100000) && (trgger_arm == 1))) // counter less than 1.2ms
			begin
			counter <= counter + 14'b00000000000001;
			trgger_arm <= 1;
			
			end
		else if (trgger == 0)
			begin
			trgger_arm <= 0;
			counter <= 14'b00000000000000;
			end
		end
		
		
		
		
		
	///data sender	
	always@ (negedge TenMHzClock)
		begin
		//after trigger and .8ms has passed, sends out flag signal, sends swith data to output reg
		//and sets up flag to send data
		if ((14'b0000000000001000 <= counter) && (counter < 14'b0000000000001001)) //counter btwn .8ms and .9 ms
			begin
			dflag <=1;
			
			data_output_reg <= data_reg;
			trgger_flag <= 1;
			end
		//after 1 ms after the trigger the data is transfered to the pin connected to data_output
		//then after all 18 bits this block is bypass and signal is set low
		if ((counter >= 14'b0000000000001010) && (trgger_flag == 1))// counter greater than or equal to 1ms
			begin
			dflag <=0;
			data_output <= data_output_reg[M];
				if (M <= 16)
					begin
					M <= M + 1;
					end
				else
					begin
					trgger_flag <= 0;
					data_output <= 0;
					end
			end
		
		//after .1ms a .1ms flag signal is send to mark end of transmission		
		/*10+18 for testbtwn 29-30*/	
		if ((14'b0000000000011101 <= counter) && ( counter < 14'b0000000000011110)) //counter btwn 1.1ms and 1.2ms
			begin
			dflag <= 1;
			M <= 0;
			end
		
		//dflag stays low unless the message is about to be send or ends	
		else
			begin
			dflag <= 0;
			end
			
		end
			
			
	always@ (posedge TenMHzClock)
		begin
		if (trgger_flag == 0)
			begin
		
			data_reg[0] <= swtch0;
			data_reg[1] <= swtch1;
			data_reg[2] <= swtch2; 
			data_reg[3] <= swtch3; 
			data_reg[4] <= swtch4; 
			data_reg[5] <= swtch5; 
			data_reg[6] <= swtch6; 
			data_reg[7] <= swtch7; 
			data_reg[8] <= swtch8; 
			data_reg[9] <= swtch9; 
			data_reg[10] <= swtch10; 
			data_reg[11] <= swtch11; 
			data_reg[12] <= swtch12; 
			data_reg[13] <= swtch13; 
			data_reg[14] <= swtch14; 
			data_reg[15] <= swtch15; 
			data_reg[16] <= swtch16; 
			data_reg[17] <= swtch17;
			end
		
		end
		
endmodule

			
			
			
	



	
	
		
	
		
		
			
		
	
	