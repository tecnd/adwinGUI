module reader(SCLK_PE3, SDIO_PE5, FTWhigh_32bit);

	input SCLK_PE3;  
	input  SDIO_PE5;
	
	
	output reg [31:0] FTWhigh_32bit;
	
	integer N;  //count when to start reading
	integer M; // bit in register number
	
	reg [0:31] temp_high;
	
	initial
		begin
		N<=0;
		M<=0;
		end
	
	always@ (posedge SCLK_PE3)
		begin
		N <= N + 1;
		if ( N >= 183 )
			begin
			N<=0;
			end
		else
		if ( N >= 152)
			begin
			M<=0;
			FTWhigh_32bit <= temp_high;
			end
			
		else 
		if ( N >= 120 )
			begin
			temp_high[M] <= SDIO_PE5;
			if (M <= 31)
				begin
				M<=M+1;
				end
				
			end
		end
	
			
endmodule			
	