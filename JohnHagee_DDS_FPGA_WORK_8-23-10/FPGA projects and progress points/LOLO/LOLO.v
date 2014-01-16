/***
This project makes the DDS have a 4 ns blip.
INIT_REG, and UNCLEAR_REG are the initialize registers from the main program.  SWEEP_REG_1 is the first sample sweep.  
These are combined into one register "FIRST_REG" only for the simplicity of writing code to test the solution

LOLO_REG is a register setting the lower limit of the first sweep to the higher and lower limits

SWEEP_REG_2 is the second sample sweep. Again, these are combined into "SECOND_REG"
to making the coding simpler for this test.

In an actual implementation, the limits wold have to be read from the rabbit and 
arranged to fill up the appropriate registers from user input which could be somewhat complicated.

SEE notes below for further explanation

***/
module LOLO(tenMHz_ext, key_3_sweep, key_0_init, SDIO, SCLK, IO_UPDATE, IO_RESET, 
DR_CTL,  OSK, CSB, DR_HOLD,scope_trigger, sweep_end_flag, sweep_key_flag, init_end_flag, init_key_flag);

input tenMHz_ext;
input key_3_sweep;
input key_0_init;
output reg SDIO;
output reg SCLK;
output reg IO_UPDATE;
output reg  IO_RESET;
output reg DR_CTL;
output reg  OSK;
output reg CSB;
output reg DR_HOLD;
output reg scope_trigger;

output reg sweep_end_flag;
output reg sweep_key_flag;
output reg init_end_flag;
output reg init_key_flag;

integer T;
integer N;
integer M;
integer B;
integer P;




/*
parameter [0:231] INIT_REG = {8'h0b,32'h0,32'h0,8'h00,32'h00001000,
8'h01,32'h00480020,8'h02,32'h353fc1c8,8'h03,32'h0};
parameter [0:39] UNCLEAR_REG = {8'h00,32'h00000000};
parameter [0:183] SWEEP_REG_1 = {8'h0c,32'h00000011,32'hffffffff,8'h0d,16'h0001,16'h0001,
8'h0b,32'h33333333,32'h19999999};
parameter [0:71] LOLO_REG = {8'h0b,32'h19999999,32'h19999999};
parameter [0:183] SWEEP_REG_2 = {8'h0c,32'h0000000a,32'hffffffff,8'h0d,16'h000c,16'h0001,
8'h0b,32'h19999999,32'h0ccccccc};*/


parameter [0:455] FIRST_REG = {/*[0:231] INIT_REG =*/ 8'h0b,32'h0,32'h0,8'h00,32'h00001000,
8'h01,32'h00480020,8'h02,32'h353fc1c8,8'h03,32'h0,/*[232:271] UNCLEAR_REG = */8'h00,32'h00000000,
/*[272:455] SWEEP_REG_1 = */8'h0c,32'h00000508,32'hffffffff,8'h0d,16'h0001,16'h0001,
8'h0b,32'h07ae147a,32'h03d70a3d};
parameter [0:255] SECOND_REG = {/*[0:71] LOLO_REG = */8'h0b,32'h03d70a3d,32'h03d70a3d,
/*[72:255] SWEEP_REG_2 = */8'h0c,32'h000002af,32'hffffffff,8'h0d,16'h0001,16'h0001,
8'h0b,32'h03d70a3d,32'h01cac083};





always@ (posedge tenMHz_ext)
	begin
	OSK<=0;
	//DR_HOLD<=0;
	CSB<=0;
	scope_trigger<= IO_UPDATE/*!(key_3_sweep & key_0_init)*/;
	if ((key_3_sweep!= 1) && (sweep_end_flag == 0))
		begin
		sweep_key_flag <=1;
		end
	if (sweep_end_flag != 0)
		begin
		sweep_key_flag <=0;
		end
		
	if ((key_0_init != 1) && (init_end_flag == 0))
		begin
		init_key_flag <=1;
		end
	if (init_end_flag !=0)
		begin
		init_key_flag <=0;
		end
		
	if 	(sweep_key_flag!=0)
		begin
		T<=T+1;
		end
		
	
		
	if (init_key_flag != 0)
		begin
		
				
		//B increments while init_key_flag is high. when B 
		B<=B+1'b1;
		
		end
		
	if (((B>= 568) && (B<573)) || ((B>=751) && (B<756)) || ((B>=1217) && (B<1218)))
		begin
		IO_UPDATE <= 1;
		end
	if ((((B<568) || (B>=573))&&((B<751)||(B>=756)) && ((B<1217) || (B>=1218)))&&(sweep_key_flag == 0))
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
	if (((B>=55) && (B<519))/*INIT_REG is sent out */ || ((B>=622) && (B<702)) 
	/*unclear reg is sent out*/|| ((B>=800) && (B<1168))/* first sweep is sent out and executed by DR_CTL and 
	IO_UPDATE being used at the proper time*/)
		begin
		if(M>=1)
			begin
			M<=0;
			P<=P+1;
			SCLK<=1;
			end
		if(M<1)
			begin
			M<=M+1'b1;
			SCLK<=0;
			SDIO <= FIRST_REG[P];
			end
		end
	
	if ((((B<55) || (B>=519)) && ((B<622)||(B>=702)) && ((B<800) || (B>=1168
	))) && (sweep_key_flag == 0))
		begin
		SCLK<=0;
		SDIO<=0;
		end
	if (B<55)
		begin
		P<=0;
		end
	if ((B>=1216) && (B<1225))
		begin
		DR_CTL<=1;
		end
	if (((B<770)||(B>=1225))&&(sweep_key_flag == 0))
		begin
		DR_CTL<=0;
		end
		
	if (B==20000000)
		begin
		init_end_flag<=1;
		
		B<=0;
		end
	if (B<20000000)
		begin
		init_end_flag<=0;
		end
		
		
		
		
		/***************
		critical part!!!!***************************/
	if (((T>=5) && (T<149))/*this is where both limits are set to the lower limit */ || ((T>=210) && (T<578))
	/*This is where the second sweep s entered*/)
		begin
		if(M>=1)
			begin
			M<=0;
			N<=N+1;
			SCLK<=1;
			end
		if(M<1)
			begin
			M<=M+1'b1;
			SCLK<=0;
			SDIO <= SECOND_REG[N];
			end
		end
	if ((((T<5) && (T>=149)) || ((T<210) && (T>=578))) && (init_key_flag == 0))
		begin
		SCLK<=0;
		SDIO<=0;
		end
		
		
	if (T<0)
		begin
		N<=0;
		end
	if ((T>=580) && (T<595))
		begin
		DR_HOLD<=1;
		end
	if ((T<580) || (T>=595))
		begin
		DR_HOLD<=0;
		end
		
	if (((T>= 200) && (T<201)) || ((T>=590) && (T<591)))
		begin
		IO_UPDATE<=1;
		end
		
		
	if ((((T<200) || (T>=201)) && ((T<590) || (T>=591))) && (init_key_flag == 0))
		begin
		IO_UPDATE<=0;
		end
		/*********
		critical part!!!!!!****/
	if ((T>=589) && (T<598))///DR_CTL is set high for the high to low sweep ONE CLOCK TICK *BEFORE*
	//// IO_UPDATE is toggled (DR_DTL @589 and IO_UPDATE @590 this how to take advantage of 
	//the fact that both limits are set to the lower limit of the first sweep.
		begin
		DR_CTL<=1;
		end
	if (((T<489) || (T>=598)) && (init_key_flag == 0))
		begin
		DR_CTL<=0;
		end
	if (T==20000000)
		begin
		sweep_end_flag<=1;
		T<=0;
		end
	if (T<20000000)
		begin
		sweep_end_flag<=0;
		end
		
		
	end
endmodule
		