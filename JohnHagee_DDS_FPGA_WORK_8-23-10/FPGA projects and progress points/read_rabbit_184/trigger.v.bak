module trigger(key0, Fifty_MHz_clock, trigger_pulse_out);



	input Fifty_MHz_clock;
	
	input key0;
	


	output reg trigger_pulse_out;
	
	

	
	always@ (posedge Fifty_MHz_clock)
		begin
		
				
		if (key0 == 1)
			begin
			
			trigger_pulse_out <= 0;
			
			
			end
			
		if (key0 == 0)
			begin
			
			trigger_pulse_out <= 1;
			
			
			end
			
			
	
		end
				
			
			
			
			
			
endmodule
