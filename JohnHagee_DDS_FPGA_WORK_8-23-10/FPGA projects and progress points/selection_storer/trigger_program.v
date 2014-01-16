//pretty simple, just sends to separate signals based on two speparate keys
	
	module trigger_program(fifty_MHz_clock, key0, key2, send_trigger_output, display_trigger_output);

	input fifty_MHz_clock;
	input key0;
	input key2;
	
	output reg send_trigger_output;
	output reg display_trigger_output;
	
	always@ (posedge fifty_MHz_clock)
		begin
		
				
		if (key0 == 1)
			begin
			
			send_trigger_output <= 0;
			
			end
			
		if (key0 == 0)
			begin
			
			send_trigger_output <= 1;
			
			end
			
		if (key2 == 1)
			begin
			
			display_trigger_output <= 0;
			
			end
			
		if (key2 == 0)
			begin
			
			display_trigger_output <= 1;
			
			end
			
			
					
		
		
		end
				
			
			
			
			
			
endmodule
	
	