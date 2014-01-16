module InitializeSynclcok(TenMHzExt, key_0_init, SDIO,SCLK,init_key_flag,
IO_UPDATE,
IO_RESET,
init_end_flag,DR_HOLD,OSK,CSB);

input TenMHzExt;
input key_0_init;

output reg SDIO;
output reg SCLK;
output reg init_key_flag;
output reg IO_UPDATE;
output reg IO_RESET;
output reg init_end_flag;
output reg DR_HOLD;
output reg OSK;
output reg CSB;

reg R;
reg Q;
reg [8:0] P;
integer B;


parameter [0:271] INIT_REG = {8'h0b,32'h0,32'h0,128'h0000004000010048002002353fc1c803,32'h0,8'h0a,32'h04000000};




always@ (posedge TenMHzExt)
	begin
	CSB <=0;//must be tied to zero to have DDS recieve
	OSK <=0;
	DR_HOLD <=0;
	
	if (init_key_flag != 0)
		begin
		
				
		//B increments while init_key_flag is high. when B 
		B<=B+1'b1;
		
		end
		
	if ((B>= 608) && (B<613))
		begin
		IO_UPDATE <= 1;
		end
	if ((B<608) || (B>=613))
		begin
		IO_UPDATE <=0;
		end	
	if ((B>=2) && (B<7))
		begin
		IO_RESET <= 1;
		
		end
	if ((B<2) || (B>=7))
		begin
		IO_RESET <=0;
		end
	if ((B>=55) && (B<559))
		begin
		if(Q>=1)
			begin
			Q<=0;
			SCLK<=1;
			end
		if(Q<1)
			begin
			Q<=Q+1'b1;
			SCLK<=0;
			end
		if (R < 1)
			begin
			R<=R+1'b1;
			SDIO <= INIT_REG[P];
			end
		if (R >= 1)
			begin
			P<= P+1'b1;
			
			R<=0;
			end
		end
	if ((B<55) || (B>=559))
		begin
		P<=0;
		SCLK<=0;
		SDIO<=0;
		end
		
	if (B>=/*1000*/4000000)
		begin
		init_end_flag<=1;
		B<=0;
		end
		
	if (B</*1000*/4000000)
		begin
		init_end_flag<=0;
		end
		
	if ((key_0_init == 0) && (init_end_flag == 0))
		begin
		init_key_flag <=1;
		end
	if (init_end_flag != 0)
		begin
		init_key_flag <=0;
		end
		
	end 
endmodule
