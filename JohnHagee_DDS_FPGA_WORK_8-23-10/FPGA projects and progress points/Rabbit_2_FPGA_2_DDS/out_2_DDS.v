module out_2_DDS(ten_MHz_ext_0, key_0_init_0, key_3_sweep_0, key_1_trigger_0, full_184_bit_0,
SDIO_0, SCLK_0,DR_CTL_0, IO_UPDATE_0,IO_RESET_0,CSB_0, trigger_0, DR_HOLD_0, OSK_0, init_key_flag);


input ten_MHz_ext_0;
input key_0_init_0;
input key_3_sweep_0;
input key_1_trigger_0;
input [0:183] full_184_bit_0;

output reg SDIO_0;
output reg SCLK_0;
output reg DR_CTL_0;
output reg IO_UPDATE_0;
output reg IO_RESET_0;
output reg CSB_0;
output reg trigger_0;
output reg DR_HOLD_0;
output reg OSK_0;


integer M;
integer N;
integer L;
integer T;

reg init_end_flag;
output reg init_key_flag;

reg sweep_end_flag;
reg sweep_key_flag;




parameter [0:231] INIT_REG = {8'h0b,32'd0,32'd0,128'h0000004000010048002002353fc1c803,32'd0};

initial  
	begin
	T <= 0;
	M <= 0;
	N <= 0;
	L <= 0;
	SDIO_0 <= 0;
	SCLK_0 <= 0;
	
	IO_UPDATE_0 <= 0;
	IO_RESET_0 <=0;
	init_end_flag <=0;
	init_key_flag <=0;
	sweep_end_flag <=0;
	sweep_key_flag <=0;
	end
	
	
always@ (negedge ten_MHz_ext_0)
	begin
	CSB_0 <=0;
	OSK_0 <=0;
	DR_HOLD_0 <=0;
	if ((key_0_init_0 != 0) && (init_end_flag == 0))
		begin
		init_key_flag <=1;
		end
	if (init_end_flag != 0)
		begin
		init_key_flag <=0;
		end
	if ((key_3_sweep_0 != 1) && (sweep_end_flag == 0))
		begin
		sweep_key_flag <=1;
		end
	if (sweep_end_flag != 0)
		begin
		sweep_key_flag <=0;
		end
	end
	
always@ (posedge ten_MHz_ext_0)
	begin
	if ((init_key_flag != 0) || (sweep_key_flag != 0))
		begin
		T <=T+1;
		end
		
	if (init_key_flag != 0)
		begin
		if ((T>= 568) && (T<573))
			begin
			IO_UPDATE_0 <= 1;
			end
		if ((T<568) || (T>=573))
			begin
			IO_UPDATE_0 <=0;
			end	
		if ((T>=2) && (T<7))
			begin
			IO_RESET_0 <= 1;
			end
		if ((T<2) || (T>=7))
			begin
			IO_RESET_0 <=0;
			end
		if ((T>=55) && (T<519))
			begin
			if(L>=1)
				begin
				L<=0;
				SCLK_0<=1;
				end
			if(L<1)
				begin
				L<=L+1;
				SCLK_0<=0;
				end
			if (M < 1)
				begin
				M<=M+1;
				SDIO_0 <= INIT_REG[N];
				end
			if (M >= 1)
				begin
				N<= N+1;
				M<=0;
				end
			end
		if ((T<55) || (T>=519))
			begin
			N<=0;
			SCLK_0<=0;
			SDIO_0<=0;
			end
		end	
	if (sweep_key_flag != 0)
		begin
		if ((T>= 422) && (T<427))
			begin
			IO_UPDATE_0 <= 1;
			end
		if ((T<422) || (T>=427))
			begin
			IO_UPDATE_0 <=0;
			end	
		if ((T>=5) && (T<373))
			begin
			if(L>=1)
				begin
				L<=0;
				SCLK_0<=1;
				end
			if(L<1)
				begin
				L<=L+1;
				SCLK_0<=0;
				end
			if (M < 1)
				begin
				M<=M+1;
				SDIO_0 <= full_184_bit_0[N];
				end
			if (M >= 1)
				begin
				N<= N+1;
				M<=0;
				end
			end
		if ((T<5) || (T>=373))
			begin
			N<=0;
			SCLK_0<=0;
			SDIO_0<=0;
			end
		if ((T>=446) && (T<451))
			begin
			if (full_184_bit_0[8:39] > full_184_bit_0[40:71])
				begin
				DR_CTL_0<=0;
				end
			if (full_184_bit_0[8:39] < full_184_bit_0[40:71])
				begin 
				DR_CTL_0<=1;
				end
			end
		if ((T<446) || (T>=451))
			begin
			if (full_184_bit_0[8:39] > full_184_bit_0[40:71])
				begin
				DR_CTL_0<=1;
				end
			if (full_184_bit_0[8:39] < full_184_bit_0[40:71])
				begin
				DR_CTL_0<=0;
				end	
			end	
		end
	if ((T>=20000000) && (T<20000001))
		begin
		sweep_end_flag <= 1;
		init_end_flag <=1;
		T<=0;
		end
	if ((T<20000000) || (T>= 20000001))
		begin
		sweep_end_flag <=0;
		init_end_flag<=0;
		end
		
		
	if (key_1_trigger_0 == 1)
		begin
		trigger_0 <= 0;
		end
			
	if (key_1_trigger_0 == 0)
		begin
		trigger_0 <= 1;
		end
		
	end
endmodule

		