module reader_multi_II(init_key_flag, SCLK_PE_3_1, SDIO_PE_5_1, full_184_bit_1);

	input SCLK_PE_3_1;  
	input  SDIO_PE_5_1;
	input init_key_flag;
	
	
	output reg [0:183] full_184_bit_1;
	
	integer N;  //count when to start reading
	integer M; // bit in register number
	
	reg [0:182] temp_184;
	
	initial
		begin
		N<=0;
		M<=0;
		end
	
	always@ (posedge SCLK_PE_3_1)
		begin
		if (init_key_flag==0)
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
				full_184_bit_1 <= {temp_184,SDIO_PE_5_1};
				end
				
			else 
			//if ( N >= 120 )
				begin
				temp_184[M] <= SDIO_PE_5_1;
				//if (M <= 183)
				//begin
				M<=M+1;
				//end
				
				end
			end
		end
			
endmodule			
	