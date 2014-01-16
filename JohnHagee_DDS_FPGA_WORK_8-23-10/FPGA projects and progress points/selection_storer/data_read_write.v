//toughest program 
//must decide whether or not to do a bunch of stuff and store some numbers

//inputs:
	//ten_MHz_synch_in clock for everything, especially reading the numbers
	
	//data_ctrl_in -- signals while sig is being sent
	
	//serial_in -- data number in
	
	//queue0 -- cable to tell which thing to do what to
	
	//displaying_trigger_in -- triggers to send out display -- which one is determined by queue
	
	


module data_read_write (ten_MHz_synch_in, data_ctrl_in, serial_in, queue0, safe_switch, displaying_trigger_in, 
Hex_display_no);

	input ten_MHz_synch_in;
	input data_ctrl_in;
	input serial_in;
	input [2:0] queue0;
	input safe_switch;
	input displaying_trigger_in;
	
	
	
	output reg [11:0] Hex_display_no;
	
	
	integer N;				//integer to give place in 
	
	
	// NEED TO FIGURE OUT WHAT TO DO WITH DATA REGS????
	//M/A7BE MULTIPLE ONES AND A CHOICE POINT FOR EVERYTHING
	
	
	reg [11:0] data_reg0;	//12-bit register to have data read into it
	reg [11:0] data_reg1;
	reg [11:0] data_reg2;
	reg [11:0] data_reg3;
	reg [11:0] data_reg4;
	reg [11:0] data_reg5;
	reg [11:0] data_reg6;
	reg [11:0] data_reg7;
	/*reg flagg;	*/
	
	
	
	always@ (posedge ten_MHz_synch_in)
		begin
		if (/*(*/(data_ctrl_in == 1) && (N <= 11) && (safe_switch == 0)/*) || (flagg == 1)*/ )
			begin
			
			if (queue0 == 000)
				begin
				data_reg0[N] <= serial_in;
				end
			if (queue0 == 001)
				begin
				data_reg1[N] <= serial_in;
				end
			if (queue0 == 010)
				begin
				data_reg2[N] <= serial_in;
				end
			if (queue0 == 011)
				begin
				data_reg3[N] <= serial_in;
				end
			if (queue0 == 100)
				begin
				data_reg4[N] <= serial_in;
				end
			if (queue0 == 101)
				begin
				data_reg5[N] <= serial_in;
				end
			if (queue0 == 110)
				begin
				data_reg6[N] <= serial_in;
				end
			if (queue0 == 111)
				begin
				data_reg7[N] <= serial_in;
				end
			
			  //as N cycles from 0 to 11
			/*flagg <= 1;*/
			N <= N + 1;
			end
			
			
		if (( N >= 12) && /*(flagg == 1) && */ (displaying_trigger_in == 0))
			begin
			
			if (queue0 == 000)
				begin
				Hex_display_no <= data_reg0;
				end
			if (queue0 == 001)
				begin
				Hex_display_no <= data_reg1;
				end
			if (queue0 == 010)
				begin
				Hex_display_no <= data_reg2;
				end
			if (queue0 == 011)
				begin
				Hex_display_no <= data_reg3;
				end
			if (queue0 == 100)
				begin
				Hex_display_no <= data_reg4;
				end
			if (queue0 == 101)
				begin
				Hex_display_no <= data_reg5;
				end
			if (queue0 == 110)
				begin
				Hex_display_no <= data_reg6;
				end
			if (queue0 == 111)
				begin
				Hex_display_no <= data_reg7;
				end
			N <= 0;
			//flagg<= 0;
			end	
			
		end
		
endmodule
	
	
			
			