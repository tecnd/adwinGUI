module out_2_DDS(ten_MHz_ext_0, key_0_init_0, key_2_reset_0 ,key_3_sweep_0, key_1_trigger_0, full_184_bit_0_0,
full_184_bit_0_1, 
full_184_bit_0_2,
full_184_bit_0_3,
full_184_bit_0_4,
full_184_bit_0_5,
full_184_bit_0_6,
full_184_bit_0_7,
full_184_bit_0_8,
full_184_bit_0_9,
full_184_bit_0_10,
full_184_bit_0_11,/*
full_184_bit_0_12,
full_184_bit_0_13,
full_184_bit_0_14,
full_184_bit_0_15,
full_184_bit_0_16,
full_184_bit_0_17,
full_184_bit_0_18,
full_184_bit_0_19,*/
sweep_total_0,
SDIO_0, SCLK_0,DR_CTL_0, IO_UPDATE_0,IO_RESET_0,CSB_0, trigger_0, DR_HOLD_0,
 OSK_0, sweep_count_over_flag_0,init_key_flag,p_number,tell_sweep_flag);


input ten_MHz_ext_0;
input key_0_init_0;
input key_3_sweep_0;
input key_2_reset_0 ;
input key_1_trigger_0;
input [0:183] full_184_bit_0_0;
input [0:183] full_184_bit_0_1;
input [0:183] full_184_bit_0_2;
input [0:183] full_184_bit_0_3;
input [0:183] full_184_bit_0_4;
input [0:183] full_184_bit_0_5;
input [0:183] full_184_bit_0_6;
input [0:183] full_184_bit_0_7;
input [0:183] full_184_bit_0_8;
input [0:183] full_184_bit_0_9;
input [0:183] full_184_bit_0_10;
input [0:183] full_184_bit_0_11;/*
input [0:183] full_184_bit_0_12;
input [0:183] full_184_bit_0_13;
input [0:183] full_184_bit_0_14;
input [0:183] full_184_bit_0_15;
input [0:183] full_184_bit_0_16;
input [0:183] full_184_bit_0_17;
input [0:183] full_184_bit_0_18;
input [0:183] full_184_bit_0_19;*/
input [4:0] sweep_total_0;

output reg SDIO_0;
output reg SCLK_0;
output reg DR_CTL_0;
output reg IO_UPDATE_0;
output reg IO_RESET_0;
output reg CSB_0;
output reg trigger_0;
output reg DR_HOLD_0;
output reg OSK_0;
output reg sweep_count_over_flag_0;

output reg [25:0] p_number;
output reg tell_sweep_flag;


integer M;
integer N;
integer L;
integer P;
integer T;
//integer Q;


reg [0:183] full_184_bit_0;
reg init_end_flag;
output reg init_key_flag;

reg sweep_end_flag;
reg sweep_key_flag;

reg total_end_flag;




parameter [0:231] INIT_REG = {8'h0b,32'd0,32'd0,128'h0000004000010048002002353fc1c803,32'd0};

initial  
	begin
	T <= 0;
	M <= 0;
	N <= 0;
	L <= 0;
	P <=0;
	SDIO_0 <= 0;
	SCLK_0 <= 0;
	
	IO_UPDATE_0 <= 0;
	IO_RESET_0 <=0;
	init_end_flag <=0;
	init_key_flag <=0;
	sweep_end_flag <=0;
	sweep_key_flag <=0;
	total_end_flag <=0;
	sweep_count_over_flag_0<=0;
	end
	
	
always@ (negedge ten_MHz_ext_0)
	begin
	CSB_0 <=0;
	OSK_0 <=0;
	DR_HOLD_0 <=0;
	tell_sweep_flag<=sweep_key_flag;
	//Q<=sweep_total_0+1;
	
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
		sweep_count_over_flag_0<=1;
		end
	if (key_2_reset_0 !=1)
		begin
		sweep_count_over_flag_0<=0;
		P<=0;
		
		end
		
	if ((sweep_end_flag != 0) && (P<(sweep_total_0+1)))
		begin
		P<=P+1;
		p_number<=P;
		sweep_key_flag <=0;
		end
	if ((P>=sweep_total_0) && (sweep_total_0 != 27))
		begin
		total_end_flag <=1;
		sweep_key_flag<=0;
	
		end
	if ((P<sweep_total_0) || (sweep_total_0 == 27))
		begin
		total_end_flag <=0;
		end
	end
	
always@ (posedge ten_MHz_ext_0)
	begin
	if (total_end_flag <= 0)
		begin
		if ((init_key_flag != 0) || (sweep_key_flag != 0))
			begin
			T <=T+1;
			end
		
		if (init_key_flag != 0)
			begin
			if ((T>= 1136) && (T<1146))
				begin
				IO_UPDATE_0 <= 1;
				end
			if ((T<1136) || (T>=1146))
				begin
				IO_UPDATE_0 <=0;
				end	
			if ((T>=4) && (T<14))
				begin
				IO_RESET_0 <= 1;
				end
			if ((T<4) || (T>=14))
				begin
				IO_RESET_0 <=0;
				end
			if ((T>=110) && (T<1038))
				begin
				if(L>=3)
					begin
					L<=0;
					SCLK_0<=1;
					end
				if(L<2)
					begin
					L<=L+1;
					SCLK_0<=0;
					end
				if (M < 2)
					begin
					M<=M+1;
					SDIO_0 <= INIT_REG[N];
					end
				if (M >= 3)
					begin
					N<= N+1;
					M<=0;
					end
				end
			if ((T<110) || (T>=1038))
				begin
				N<=0;
				SCLK_0<=0;
				SDIO_0<=0;
				end
			end	
		if (sweep_key_flag != 0)
			begin
			case (P)
				1: full_184_bit_0 = full_184_bit_0_0; 
				2: full_184_bit_0 = full_184_bit_0_1;
				3: full_184_bit_0 = full_184_bit_0_2;
				4: full_184_bit_0 = full_184_bit_0_3;
				5: full_184_bit_0 = full_184_bit_0_4;
				6: full_184_bit_0 = full_184_bit_0_5;
				7: full_184_bit_0 = full_184_bit_0_6;
				8: full_184_bit_0 = full_184_bit_0_7;
				9: full_184_bit_0 = full_184_bit_0_8;
				10: full_184_bit_0 = full_184_bit_0_9;
				11: full_184_bit_0 = full_184_bit_0_10;
				12: full_184_bit_0 = full_184_bit_0_11;/*
				12: full_184_bit_0 = full_184_bit_0_12;
				13: full_184_bit_0 = full_184_bit_0_13;
				14: full_184_bit_0 = full_184_bit_0_14;
				15: full_184_bit_0 = full_184_bit_0_15;
				16: full_184_bit_0 = full_184_bit_0_16;
				17: full_184_bit_0 = full_184_bit_0_17;
				18: full_184_bit_0 = full_184_bit_0_18;
				19: full_184_bit_0 = full_184_bit_0_19;*/
			endcase
			if ((T>= 844) && (T<852))
				begin
				IO_UPDATE_0 <= 1;
				end
			if ((T<844) || (T>=852))
				begin
				IO_UPDATE_0 <=0;
				end	
			if ((T>=10) && (T<746))
				begin
				if(L>=3)
					begin
					L<=0;
					SCLK_0<=1;
					end
				if(L<2)
					begin
					L<=L+1;
					SCLK_0<=0;
					end
				if (M < 2)
					begin
					M<=M+1;
					/*if (P==19)
						begin
						SDIO_0 <= full_184_bit_0_19[N];
						end
					if (P==18)
						begin
						SDIO_0 <= full_184_bit_0_18[N];
						end
					if (P==17)
						begin
						SDIO_0 <= full_184_bit_0_17[N];
						end
					if (P==16)
						begin
						SDIO_0 <= full_184_bit_0_16[N];
						end
					if (P==15)
						begin
						SDIO_0 <= full_184_bit_0_15[N];
						end
					if (P==14)
						begin
						SDIO_0 <= full_184_bit_0_14[N];
						end
					if (P==13)
						begin
						SDIO_0 <= full_184_bit_0_13[N];
						end
					if (P==12)
						begin
						SDIO_0 <= full_184_bit_0_12[N];
						end
					if (P==11)
						begin
						SDIO_0 <= full_184_bit_0_11[N];
						end
					if (P==10)
						begin
						SDIO_0 <= full_184_bit_0_10[N];
						end
					if (P==9)
						begin
						SDIO_0 <= full_184_bit_0_9[N];
						end
					if (P==8)
						begin
						SDIO_0 <= full_184_bit_0_8[N];
						end
					if (P==7)
						begin
						SDIO_0 <= full_184_bit_0_7[N];
						end
					if (P==6)
						begin
						SDIO_0 <= full_184_bit_0_6[N];
						end
					if (P==18)
						begin
						SDIO_0 <= full_184_bit_0_18[N];
						end
					
					*/
					
					SDIO_0 <= full_184_bit_0[N];
					
					
					
					end
				if (M >= 3)
					begin
					N<= N+1;
					M<=0;
					end
				end
			
			if ((T>=892) && (T<902))
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
			if ((T<892) || (T>=902))
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
			if ((T<10) || (T>=746))
				begin
				N<=0;
				SCLK_0<=0;
				SDIO_0<=0;
				end
			end
		if ((T>=20000000) && (T<20000001))
			begin
			if (sweep_key_flag !=0)
				begin
				sweep_end_flag <= 1;
				end
			init_end_flag <=1;
			T<=0;
			end
		if ((T<20000000) || (T>= 20000001))
			begin
			sweep_end_flag <=0;
			init_end_flag<=0;
			end
		
		
		
		
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

		