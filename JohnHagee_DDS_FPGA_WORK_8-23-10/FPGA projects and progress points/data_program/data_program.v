module data_program(trgger, swtches, TenMHzToData, data_output, dflag);
	
	input TenMHzToData;
	input trgger;
	input [17:0] swtches;
	
	output reg data_output;
	output reg dflag;
	
	
	
	reg trgger_flag;
	reg trgger_arm;
	//reg [17:0] data_reg;
	//reg [17:0] data_output_reg;
	//integer M;
	reg [13:0] counter;
	
	//when trigger is detected at the same time arm is off counter will start and arm will be on
	//when counter is above the time at which everything is done, block will be bypassed
	//as long as the trigger is low, arm and counter will be reset to wait for another trigger
	//always@ (posedge TenMHzToData)
		//begin
		//if ((trgger == 1) & (trgger_arm == 0)) // counter less than 1.2ms
			//begin
			//counter = counter + 14'b00000000000001;
			//trgger_arm = 1;
			//if (counter >= 10111011100000)
				//begin
				//counter = 14'b00000000000000;
				//end
			
			//end
		//else if (trgger == 0)
//			begin
			//trgger_arm = 0;
			//counter = 14'b00000000000000;
			//end
		//end
		
		
		
		
		
	///data sender	
	//always@ (negedge TenMHzToData)
		//begin
		//after trigger and .8ms has passed, sends out flag signal, sends swith data to output reg
		//and sets up flag to send data
		//if (14'b1111101000000 < counter < 14'b10001100101000) //counter btwn .8ms and .9 ms
//			begin
			//dflag = 1;
//			
			//data_output_reg = data_reg;
			//trgger_flag = 1;
			//end
		//after 1 ms after the trigger the data is transfered to the pin connected to data_output
		//then after all 18 bits this block is bypass and signal is set low
//		if ((counter >= 14'b10011100010000) & (trgger_flag == 1))// counter greater than or equal to 1ms
	//		begin
		//	dflag = 0;
			//data_output = data_output_reg[M];
				//if (M <= 16)
//					begin
	//				M = M + 1;
		//			end
			//	else
				//	begin
					//trgger_flag = 0;
//					da//ta_output = 0;
	//				end
		//	end
		
		//after .1ms a .1ms flag signal is send to mark end of transmission			
//		if (14'b10101011111000 < counter < 14'b10111011100000) //counter btwn 1.1ms and 1.2ms
	//		begin
		//	dflag = 1;
			//M = 0;
//			end
		
		//dflag stays low unless the message is about to be send or ends	
	//	else
		//	begin
			//dflag = 0;
			//end
			
		//end
			
			
	//always@ (posedge TenMHzToData)
	//	begin
		//if (trgger_flag == 0)
			//begin
		
		//	data_reg = swtches;
			//end
		
		//end
		
endmodule

			
			
			
	



	
	
		
	
		
		
			
		
	
	