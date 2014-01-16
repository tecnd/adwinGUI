module reader(SCLK_PE3, SDIO_PE5, whole_message_184bit);

	input SCLK_PE3;  
	input  SDIO_PE5;
	
	
	output reg [183:0] whole_message_184bit;
	
	integer N;  //count when to start reading
	integer M; // bit in register number
	
	reg [0:182] temp_184;
	
	initial
		begin
		N<=0;
		M<=0;
		end
	
	always@ (posedge SCLK_PE3)
		begin
		N <= N + 1;
		/*if ( N >= 183 )
			begin
			N<=0;
			end
		else*/
		if ( N >= 183)
			begin
			
			M<=0;
			N<=0;
			whole_message_184bit <= {temp_184,SDIO_PE5};
			end
			
		else 
		//if ( N >= 120 )
			begin
			temp_184[M] <= SDIO_PE5;
			//if (M <= 183)
				//begin
				M<=M+1;
				//end
				
			end
		end
	
			
endmodule			
	