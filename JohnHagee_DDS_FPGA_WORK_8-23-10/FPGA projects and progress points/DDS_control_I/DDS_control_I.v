module DDS_control_I(key_0_init, key_3_sweep, key_1_trigger, sw_begin_1, sw_begin_0, sw_end_1, sw_end_0, 
fifty_MHz_intclk, /*DR_OVR,*//*ten_MHz_extclk,*/SDIO, SCLK,DR_CTL, IO_UPDATE,
IO_RESET,CSB, trigger, DR_HOLD, OSK);

input key_0_init;
input key_3_sweep;
input sw_begin_1;
input sw_begin_0;
input sw_end_1;
input sw_end_0; 
input fifty_MHz_intclk;
input key_1_trigger;

//input DR_OVR;

output reg SDIO;
output reg SCLK;
output reg DR_CTL;
output reg IO_UPDATE;
output reg CSB;
output reg IO_RESET;
output reg trigger;
output reg DR_HOLD;
output reg OSK;


integer M;
integer N;
integer L;
integer P;
integer T;

reg [3:0] LIMIT_REG;

reg init_end_flag;
reg init_key_flag;

reg sweep_end_flag;
reg sweep_key_flag;
reg [0:183] SWEEP_REG;


parameter [0:231] INIT_REG = {8'h0b,32'd0,32'd0,128'h0000004000010048002002353fc1c803,32'd0};
//backwards 
//parameter [0:231] INIT_REG = {8'hd0,32'd0,32'd0,128'h00000002008000120004400acfc8313c0,32'd0};
//parameter [0:183] SWEEP_REG = {8'h0c, 32'd4294967295, 32'd10, 8'h0d, 16'h0001, 16'h004e, 8'h0b, 32'd161061273, 32'd322122547}; 

//parameter [0:183] SWEEP_REG = {8'h0c, 32'd10, 32'd4294967295, 8'h0d, 16'h0001, 16'h004e, 8'h0b, 32'd644245094, 32'd161061273}; 
initial  
	begin
	T <= 0;
	M <= 0;
	N <= 0;
	L <= 0;
	P <= 0;
	SDIO <= 0;
	SCLK <= 0;
	LIMIT_REG <= 0;
	CSB <= 0;
	SWEEP_REG <= 0;
	IO_UPDATE <= 0;
	IO_RESET <=0;
	init_end_flag <=0;
	init_key_flag <=0;
	sweep_end_flag <=0;
	sweep_key_flag <=0;
	end
	

always@ (negedge fifty_MHz_intclk)
	begin
	CSB <=0;
	OSK <=0;
	DR_HOLD <=0;
	if ((key_0_init != 1) && (init_end_flag == 0))
		begin
		init_key_flag <=1;
		end
	if (init_end_flag != 0)
		begin
		init_key_flag <=0;
		end
	if ((key_3_sweep != 1) && (sweep_end_flag == 0)&&(SWEEP_REG !=17))
		begin
		sweep_key_flag <=1;
		end
	if (sweep_end_flag != 0)
		begin
		sweep_key_flag <=0;
		end
	end
	
always@ (posedge fifty_MHz_intclk)
	begin
	LIMIT_REG <= {sw_begin_0, sw_begin_1, sw_end_0, sw_end_1};
	if ((init_key_flag != 0) || ((sweep_key_flag != 0)&&(SWEEP_REG !=17)))
		begin
		T <=T+1;
		end
	
	if (init_key_flag != 0)
		begin
		if ((T>= 2190) && (T<2240))
		begin
		IO_UPDATE <= 1;
		end
	if ((T<2190) || (T>=2240))
		begin
		IO_UPDATE <=0;
		end	
		if ((T>=10) && (T<60))
			begin
			IO_RESET <= 1;
			end
		if ((T<10) || (T>=60))
			begin
			IO_RESET <=0;
			end
		if ((T>=540) && (T<1700))
			begin
			if(L>=4)
				begin
				L<=0;
				SCLK<=1;
				end
			if(L<4)
				begin
				L<=L+1;
				SCLK<=0;
				end
			if (M < 4)
				begin
				M<=M+1;
				end
			if (M >=2)
				begin
				SDIO <= INIT_REG[N];
				end
			if (M >= 4)
				begin
				N<= N+1;
				M<=0;
				end
			end
		if ((T<540) || (T>=1700))
			begin
			N<=0;
			SCLK<=0;
			SDIO<=0;
			end
		end
	
	if ((sweep_key_flag != 0)&&(SWEEP_REG !=17) && (T>=60) && (T<980) )
		begin
		if(L>=4)
			begin
			L<=0;
			SCLK<=1;
			end			
		if(L<4)				
			begin
			L<=L+1;
			SCLK<=0;
			end
		if (M < 4)
			begin
			M<=M+1;
			end
		if (M >=2)
			begin
			SDIO <= SWEEP_REG[N];
			end
		if (M >=4)
			begin
			N<= N+1;
			M<=0;
			end
		end
	if (((T<60) || (T>=980) )&&(sweep_key_flag != 0)&&(SWEEP_REG !=17))
		begin 
		N<=0;
		SCLK<=0;
		SDIO<=0;
		end
	/*if ((T == 59)&&(sweep_key_flag != 0)&&(SWEEP_REG !=17))
		begin 
		N<=0;
		SCLK<=1;
		SDIO<=SWEEP_REG[0];
		end*/
	if ((T>= 1470) && (T<1520) && (sweep_key_flag != 0)&&(SWEEP_REG !=17))
		begin
		IO_UPDATE <= 1;
		end
	if ((T<1470) || (T>=1520)&&(sweep_key_flag != 0)&&(SWEEP_REG !=17))
		begin
		IO_UPDATE <=0;
		end	
	if ((sweep_key_flag != 0)&&(SWEEP_REG !=17)&& (T>=1710) && (T<1760))
		begin
		if (SWEEP_REG[8:39] > SWEEP_REG[40:71])
			begin
			DR_CTL<=0;
			end
		if (SWEEP_REG[8:39] < SWEEP_REG[40:71])
			begin 
			DR_CTL<=1;
			end
		end
	if (((T<1710) || (T>=1760))&&(sweep_key_flag != 0)&&(SWEEP_REG !=17))
		begin
		if (SWEEP_REG[8:39] > SWEEP_REG[40:71])
			begin
			DR_CTL<=1;
			end
		if (SWEEP_REG[8:39] < SWEEP_REG[40:71])
			begin
			DR_CTL<=0;
			end	
		end
	/*if (sweep_key_flag == 0) 
		begin
		DR_CTL<=1;
		end*/
		
	if ((T>=50000000) && (T<50000010))
		begin
		sweep_end_flag <= 1;
		init_end_flag <=1;
		T<=0;
		end
	if ((T<50000000) || (T>= 50000010))
		begin
		sweep_end_flag <=0;
		init_end_flag<=0;
		end
		
		
	/*IO_UPDATE <=0;
	
	init_end_flag <= 0;
	sweep_end_flag <= 0;
	if (init_key_flag != 0)
		begin
		Q<= Q+1;
		if (Q<=10000)
			begin
			IO_RESET<=1;
			end
		else
			begin
		
			if(L>=4)
				begin
				L<=0;
				SCLK<=1;
				end
			if(L<4)
				begin
				L<=L+1;
				SCLK<=0;
				end
			if (M < 4)
				begin
				M<=M+1;
				end
			if (M >=3)
				begin
				SDIO <= INIT_REG[N];
				
				end
			if (M >= 4)
				begin
				P<=P+1;
				N<= N+1;
				M<=0;
				end
			if (P>=232)
				begin
				N<=0;
				init_end_flag <= 1;
				P<=0;
				IO_UPDATE <=1;
				//if (SWEEP_REG[8:39] > SWEEP_REG[40:71])
					//begin
					//DR_CTL<=1;
					//end
				//if (SWEEP_REG[8:39] < SWEEP_REG[40:71])
					//begin
					//DR_CTL<=0;
					//end	
				end
			end
		end
	else
		begin
	
		if (sweep_key_flag != 0)
			begin
			if(L>=4)
				begin
				L<=0;
				SCLK<=1;
				end
			if(L<4)
				begin
				L<=L+1;
				SCLK<=0;
				end
			if (M < 4)
				begin
				M<=M+1;
				end
			if (M >=3)
				begin
				SDIO <= SWEEP_REG[N];
				
				end
			if (M >= 4)
				begin
				P<=P+1;
				N<= N+1;
				M<=0;
				end
			//if (N>=183)
				//begin 
				//SDIO <= SWEEP_REG[183];	
				
				//end
			if (P>=184)
				begin
				N<=0;
				sweep_end_flag <= 1;
				P<=0;
				IO_UPDATE <=1;
				Q<=0;
				DR_CTL<=0;
				if (SWEEP_REG[8:39] > SWEEP_REG[40:71])
					begin
					DR_CTL<=0;
					end
				if (SWEEP_REG[8:39] < SWEEP_REG[40:71])
					begin
					DR_CTL<=1;
					end	
				end
			end
		else
			begin
			Q<=Q+1;
			if (Q>=16)
				begin
				Q<=0;
				DR_CTL <=1;
				
				if (SWEEP_REG[8:39] > SWEEP_REG[40:71])
					begin
					DR_CTL<=1;
					end
				if (SWEEP_REG[8:39] < SWEEP_REG[40:71])
					begin
					DR_CTL<=0;
					end	
				end
			end
		end*/
	
		//trigger_pulse_out <=1;
				
		if (key_1_trigger == 1)
			begin
			
			trigger <= 0;
			//pulse_flag <= 1;
			
			end
			
		if (key_1_trigger == 0)
			begin
			
			trigger <= 1;
			//pulse_flag <= 1;
			
			end
	case (LIMIT_REG)
		 14: SWEEP_REG = {8'h0c, 32'd4294967295, 32'd10, 8'h0d, 16'h0001, 16'h003b, 8'h0b, 32'd64424509, 32'd0}; 
		 13: SWEEP_REG = {8'h0c, 32'd4294967295, 32'd10, 8'h0d, 16'h0001, 16'h0014, 8'h0b, 32'd161061273, 32'd0};
		 12: SWEEP_REG = {8'h0c, 32'd4294967295, 32'd10, 8'h0d, 16'h0001, 16'h000c, 8'h0b, 32'd322122547, 32'd0};
		 11: SWEEP_REG = {8'h0c, 32'd10, 32'd4294967295, 8'h0d, 16'h003b, 16'h0001, 8'h0b, 32'd64424509, 32'd0};
		 9: SWEEP_REG = {8'h0c, 32'd4294967295, 32'd10, 8'h0d, 16'h0001, 16'h003b, 8'h0b, 32'd161061273, 32'd64424509};
		 8: SWEEP_REG = {8'h0c, 32'd4294967295, 32'd10, 8'h0d, 16'h0001, 16'h000f, 8'h0b, 32'd322122547, 32'd64424509};
		 7: SWEEP_REG = {8'h0c, 32'd10, 32'd4294967295, 8'h0d, 16'h0014, 16'h0001, 8'h0b, 32'd161061273, 32'd0};
		 6: SWEEP_REG = {8'h0c, 32'd10, 32'd4294967295, 8'h0d, 16'h003b, 16'h0001, 8'h0b, 32'd161061273, 32'd64424509};
		4: SWEEP_REG = {8'h0c, 32'd4294967295, 32'd10, 8'h0d, 16'h0001, 16'h0014, 8'h0b, 32'd322122547, 32'd161061273};
		3: SWEEP_REG = {8'h0c, 32'd10, 32'd4294967295, 8'h0d, 16'h000c, 16'h0001, 8'h0b, 32'd322122547, 32'd0};
		2: SWEEP_REG = {8'h0c, 32'd10, 32'd4294967295, 8'h0d, 16'h000f, 16'h0001, 8'h0b, 32'd322122547, 32'd64424509};
		1: SWEEP_REG = {8'h0c, 32'd10, 32'd4294967295, 8'h0d, 16'h0014, 16'h0001, 8'h0b, 32'd322122547, 32'd161061273};
		default: SWEEP_REG = 17;
	endcase
	
	
	
	end
	
		
endmodule
		
		
		
