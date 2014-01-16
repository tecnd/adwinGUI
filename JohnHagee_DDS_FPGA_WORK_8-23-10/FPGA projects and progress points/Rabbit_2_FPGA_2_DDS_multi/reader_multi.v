module reader_multi(init_key_flag,ten_MHz_ext_1,sweep_count_over_flag_1, SCLK_PE_3_1, SDIO_PE_5_1,
sweep_total_1,full_184_bit_choice_1,sweep_count_1);

	input SCLK_PE_3_1;  
	input  SDIO_PE_5_1;
	input sweep_count_over_flag_1;
	input ten_MHz_ext_1;
	input init_key_flag;
	
	output reg [0:183] full_184_bit_choice_1;
/*
output reg [0:183] full_184_bit_1_0;
output reg [0:183] full_184_bit_1_1;
output reg [0:183] full_184_bit_1_2;
output reg [0:183] full_184_bit_1_3;
output reg [0:183] full_184_bit_1_4;
output reg [0:183] full_184_bit_1_5;
output reg [0:183] full_184_bit_1_6;
output reg [0:183] full_184_bit_1_7;
output reg [0:183] full_184_bit_1_8;
output reg [0:183] full_184_bit_1_9;
output reg [0:183] full_184_bit_1_10;
output reg [0:183] full_184_bit_1_11;
output reg [0:183] full_184_bit_1_12;
output reg [0:183] full_184_bit_1_13;
output reg [0:183] full_184_bit_1_14;
output reg [0:183] full_184_bit_1_15;
output reg [0:183] full_184_bit_1_16;
output reg [0:183] full_184_bit_1_17;
output reg [0:183] full_184_bit_1_18;
output reg [0:183] full_184_bit_1_19;*/

output reg	[4:0] sweep_total_1;
output reg	[4:0] sweep_count_1;


	
integer N;  //count when to start reading
integer M; // bit in register number
integer L; //counting sweeps

reg [0:182] temp_184;

initial
	begin
	N<=0;
	M<=0;
	L<=0;
	sweep_count_1<=0;
	end
	
	
always@ (posedge SCLK_PE_3_1)
		begin
		if (init_key_flag==0)
			begin
			N <= N + 1;
	
			if ( N >= 183)
				begin
			
				M<=0;
				N<=0;
				L<=L+1;
			
				sweep_count_1<=L;
			
			
				full_184_bit_choice_1 <= {temp_184,SDIO_PE_5_1};
			
				end
			
			else 	
		
				begin
				temp_184[M] <= SDIO_PE_5_1;
			
				M<=M+1;
				
				
				end
			end
		
		end
always@ (posedge  ten_MHz_ext_1)
	begin
	if (sweep_count_over_flag_1!=0)
		begin
		sweep_total_1<=L;
		end
	if (sweep_count_over_flag_1 ==0)
		begin
		sweep_total_1<=27;
		end
	end
	
			
endmodule			
	