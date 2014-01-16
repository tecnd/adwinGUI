module clock_maker(FiftyMHz_ref_clock, TenMHz_receiver);
	
	input FiftyMHz_ref_clock;
	
	
	output reg TenMHz_receiver;
	
	reg [2:0] clock_counter;
	
	always@ (posedge FiftyMHz_ref_clock)
		begin
		clock_counter <= clock_counter + 3'b001;
		
		if (clock_counter >= 3'b010)
			begin
			
			TenMHz_receiver <= 1'b0;
			end
		else 
			begin
			TenMHz_receiver <= 1'b1;
			end
		if (clock_counter >= 3'b100)
			begin
			clock_counter <= 3'b000;
			end
			
		end

endmodule
