module safety_trigger(Fifty_MHz_clock, key_0, trigger_pulse_out);


//input wires:	
	input Fifty_MHz_clock;
	
	input key_0;
	
//output wires:

	output reg trigger_pulse_out;
	
	
//wires:
	
	//wire key_value;
	
//temp regs:
	
	reg pulse_flag;
	reg pulse_arm;
	reg [31:0] pulse_count;
	
	
//initialze blobk is needed so quartus will assemble the right circuit. if "pulse_flag" and "pulse_arm" aren't 
//specified, it will set them high and "simplify" the circuit so it doesn't work

	/*initial
		begin
		pulse_flag <= 0;
		pulse_arm <=0;
		end
	*/
	
//
//assign key_value = ~key_0;
		
//this loop is supposed to send a pulse when the KEY_0 is depressed -- going from 1 to 0 -- 
// it is not working probably because of an error in logic, the arning messages say that my register
//pulse_count will not increment
//I will try to get it to store an initial value for my input

	always@ (posedge Fifty_MHz_clock)
		begin
		//trigger_pulse_out <=1;
				
		if (key_0 == 1)
			begin
			
			trigger_pulse_out <= 0;
			//pulse_flag <= 1;
			
			end
			
		if (key_0 == 0)
			begin
			
			trigger_pulse_out <= 1;
			//pulse_flag <= 1;
			
			end
			
			
					
		/*if ((pulse_flag == 1) && (pulse_arm == 0))
			begin
			pulse_count <= pulse_count + 32'b0000000000000000000000000000001;
			trigger_pulse_out <= 1;
			
			if (pulse_count >= 32'b11111111111111111111111111111111) //5000*20ns = .1ms
				begin
				pulse_arm <= 1;
				trigger_pulse_out <= 0;
				end
				
			end*/
		
		end
				
			
			
			
			
			
endmodule

			