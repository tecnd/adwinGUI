


module RabbitFPGA_DRHOLD(init_trigger, key_1_trigger, key_2_hold, key_3_sweep, ten_MHz_ext, 
SDIO_PE_5, SCLK_PE_3, 
SDIO, SCLK, IO_UPDATE, DR_CTL, OSK, CSB, DR_HOLD, trigger,IO_RESET, e_number, sweep_key_flag, sweep_end_flag, complete_end_flag, 
e_flag,final_massive_stop, reset_flag, init_end_flag, init_key_flag,i_lsb,dont_read_flag, sweep_trigger_out,hold_flag, hold_end_flag,
hold_wait_flag, hold_count, d_lsb, h_lsb);

input key_3_sweep;
input ten_MHz_ext;
input SDIO_PE_5;
input SCLK_PE_3;
input key_1_trigger;
input init_trigger;
input key_2_hold;


output reg SDIO;
output reg SCLK;
output reg IO_UPDATE;
output reg DR_CTL;
output reg OSK;
output reg CSB;
output reg DR_HOLD;
output reg trigger;
output reg IO_RESET;
output reg [4:0] e_number;
output reg sweep_trigger_out;





integer J;

reg [31:0] i;  //index for the memory as it is being read in
integer N;  // index for the memory being read out
reg M; //tells when to send out the SDIO signal
reg L; //tells when to toggle the SCLK signal
reg [4:0] E;//counter for each full cycle
integer T;//time counter for sweeps
reg [10:0] B;//time counter for initialize
reg [8:0] P;//index for initialize
reg [9:0] G;//counter to time complete reset
reg [23:0] D;



reg Q;
reg R;

parameter [0:231] INIT_REG = {8'h0b,32'h0,32'h0,128'h000000400001004e002002353fc1c803,32'h0};

//parameter [0:231] INIT_REG = {8'h0b,32'h0,32'h0,128'h0000004000010048002002353fc1c803,32'h0};

output reg reset_flag;


output reg sweep_key_flag; // this is high during the time that the sweep info is being sent out to the DDS
					//it must be reset every time that each signal has been sent 
					

output reg sweep_end_flag;//this flag is: when it is set sweep_key_flag can be set back to zero


output reg complete_end_flag;  //this flag will occur when the whole memory_reg has been sent (when N>=i)
						//it resets the N and i indices to zero so the thing can be restarted
						
output reg i_lsb;



//This register is the most significant in the whole program.  The program collects the data from the rabbit and stores
// it in this register and then	reads the data from this register when sending it to the DDS.
//The register has 3680 bits because it is capable of storing 20 sweeps (20*184=3680).  However,
//not all of these bits will be used if storing less than 20 sweeps.					
reg [0:3679] memory_reg;
					
					
output reg init_end_flag;


output reg init_key_flag;		

output reg e_flag;

output reg final_massive_stop;
output reg dont_read_flag;
output reg hold_flag;
output reg hold_end_flag;
output reg hold_wait_flag;
output reg d_lsb;
output reg hold_count;
reg [23:0] H;
output reg h_lsb;


/*******testing****

reg sweep_key_flag; // this is high during the time that the sweep info is being sent out to the DDS
					//it must be reset every time that each signal has been sent 
					

reg sweep_end_flag;//this flag is: when it is set sweep_key_flag can be set back to zero


reg complete_end_flag;  //this flag will occur when the whole memory_reg has been sent (when N>=i)
						//it resets the N and i indices to zero so the thing can be restarted
						
reg [0:3679] memory_reg;
					
					
reg init_end_flag;


reg init_key_flag;		

reg e_flag;**********testing***/

initial
	begin
	i<=0;  //index for the memory as it is being read in
	N<=0;  // index for the memory being read out
	M<=0; //tells when to send out the SDIO signal
	L<=0; //tells when to toggle the SCLK signal
	T<=0;
	B<=0;//time counter for initialize
	P<=0;//index for initialize

	E<=0;
	reset_flag<=0;

	final_massive_stop<=0;
	Q<=0;
	R<=0;	
	e_flag<=0;		
	hold_wait_flag<=0;
	end				
					


//this section of code deals with making the decisions about when to start doing what
//its main job is to make sure the FPGA knows when to send the signal to the DDS
// and when to reset everything
always@ (negedge ten_MHz_ext)
	begin
	sweep_trigger_out<=!key_3_sweep;
	
	CSB <=0;//must be tied to zero to have DDS recieve
	OSK <=0;
	

	//this sets the sweep_key_flag when the key_3 is depressed and sweep_key_flag
	// has not been set in 1 sec to "debounce" the keys. sweep_end_flag is
	//set when sweep_key_flag has been set for 1 sec
	if ((key_3_sweep!= 1) && (sweep_end_flag == 0))
		begin
		sweep_key_flag <=1;
		end
	if (sweep_end_flag != 0)
		begin
		sweep_key_flag <=0;
		end
	
	if ((key_2_hold!=1)&&(hold_wait_flag==0))
		begin
		hold_flag<=1;
		end
	if (hold_end_flag !=0)
		begin
		hold_flag<=0;
		end
	
	//this sets the init_key_flag  when the rabbit sends the initalize signal
	//don't know how init_end_flag should be used
	if ((init_trigger != 0) && (init_end_flag == 0))
		begin
		init_key_flag <=1;
		end
	if (init_end_flag != 0)
		begin
		init_key_flag <=0;
		end

	if (init_trigger !=0)
		begin
		dont_read_flag<=1;
		end
	if (dont_read_flag!=0)
		begin
		J<=J+1;
		end
	if (J>=/*2000*/70000)
		begin
		J<=0;
		dont_read_flag<=0;
		end
	//this section will set the complete_end_flag which will indicate that the whole set of sweep has been run
	//this happens when N>=i this will reset everything important including setting N and i back to Zero
	if ((N>=i)&& (sweep_key_flag !=0))
		begin
		complete_end_flag <=1;
		end
	if (N<i)
		begin
		complete_end_flag<=0;
		end
	
	if (final_massive_stop !=0)
		begin
		sweep_key_flag<=0;
		end




	end






//this section of code deals with sending out the signal to the DDS 
always@ (posedge ten_MHz_ext)
	begin
	
	i_lsb<=i[0];
	d_lsb<=D[0];
	h_lsb<=H[0];
	
	/******
	Thi part of the code deals with the intialising parameters being sent to the DDS.  This specifies the settings we need on the DDS.
	
	the actual signal that must be sent to the DDS is specified above in the parameter,
	"INIT_REG = {8'h0b,32'h0,32'h0,128'h0000004000010048002002353fc1c803,32'h0}; " 
	
	The code below increments a counting register, B, which is used to time the signals being sent to
	the following pins:
		IO_RESET
		SDIO
		SCLK
		IO_UPDATE
	Note that there is no change in the DR_CTL signal because that pin only deals with actual sweeps.
	Also IO_RESET is only used during this cycle
	
	
	
	
	*****/
	
	
	//this whole section is added to try to attempt to change the ramp rate while 
	//DR_HOLD has stopped the frequency sweeping
	
	//figuring out this thing
	if (hold_flag !=0)
		begin
		hold_count<=1;
		
		
		DR_HOLD<=1;
		if (sweep_key_flag != 0)
			begin
			D<=D+1;
			end
			
		if ((D>= 422) && (D<423))
			begin
			IO_UPDATE <= 1;
			end
		if ((D<422) || (D>=423))
			begin
			IO_UPDATE <=0;
			end	
		
		if ((D>=5) && (D<229))
			begin
			if(L>=1)
				begin
				L<=0;
				SCLK<=1;
				end
			if(L<1)
				begin
				L<=L+1'b1;
				SCLK<=0;
				end
			if (M < 1)
				begin
				M<=M+1'b1;
					
				
				SDIO <= memory_reg[N];
				end
					
			if (M >= 1)
				begin
				N<= N+1'b1;
				M<=0;
				end
			end
		if ((D>=229)&&(D<373))
			begin
			if (M < 1)
				begin
				M<=M+1'b1;
					
				
				
				end
					
			if (M >= 1)
				begin
				N<= N+1'b1;
				M<=0;
				end
			end
			
		if ((D<5) || (D>=229))
			begin
			SCLK<=0;
			SDIO<=0;
			end	
		if (D>=425)
			begin
			D<=0;
			hold_end_flag<=1;
			end
			
		end
	if (hold_flag == 0)
		begin
		hold_end_flag<=0;
		DR_HOLD<=0;
		end
	if (hold_count!=0)
		begin
		H<=H+1;
		end
//////******figuring out this hold count thing!!
	
		
		
	if (init_key_flag != 0)
		begin
		
				
		//B increments while init_key_flag is high. when B 
		B<=B+1'b1;
		
		end
		
	if ((B>= 568) && (B<573))
		begin
		IO_UPDATE <= 1;
		end
	if (((B<568) || (B>=573))&&(sweep_key_flag == 0))
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
	if ((B>=55) && (B<519))
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
	if (((B<55) || (B>=519))&&(sweep_key_flag == 0))
		begin
		P<=0;
		SCLK<=0;
		SDIO<=0;
		end
		
	if (B>=1000)//testing
		begin
		init_end_flag<=1;
		B<=0;
		if (final_massive_stop!=0)
			begin
			reset_flag<=1;
			end
		end
		
	if (B<1000)
		begin
		init_end_flag<=0;
			
		
		end
		
	
	if (i<=1)
		begin
		reset_flag<=0;
		end
	


	if ((sweep_key_flag != 0)&&(final_massive_stop==0)&&(hold_flag==0))
		begin
		
		
		//T increments while sweep_key_flag is high. when T >=20,000,000 sweep_key_flag will go back to zero
		T<=T+1;
		
	
		end
		


	//when sweep_end_flag is high sweep_key_flag can be reset the number must be very large because the trigger from the keys can be >1sec
	
	





	
		
		
		
		
		
		
	//this is the section of code that toggles the IO_UPDATE
	if ((T>= 422) && (T<423))
		begin
		IO_UPDATE <= 1;
		end
	if (((T<422) || (T>=423))&&(init_key_flag == 0))
		begin
		IO_UPDATE <=0;
		end	
			
			
			
			
			
			
			
			
			
			
			
	//this is the section of code where the message is actually being sent
	//when this section is not occuring SCLK and SDIO is low
	if ((T>=5) && (T<373))
		begin
		if(L>=1)
			begin
			L<=0;
			SCLK<=1;
			end
		if(L<1)
			begin
			L<=L+1'b1;
			SCLK<=0;
			end
		if (M < 1)
			begin
			M<=M+1'b1;
				
			//the main difference with this program to have multiple sweeps
			//is that N is not reset after every sweep
			SDIO <= memory_reg[N];
			end
				
		if (M >= 1)
			begin
			N<= N+1'b1;
			M<=0;
			end
		end
	if (((T<5) || (T>=373))&&(init_key_flag == 0))
		begin
		SCLK<=0;
		SDIO<=0;
		end
			
		
	if (key_1_trigger == 1)
		begin
		trigger <= 0;
		end
			
	if (key_1_trigger== 0)
		begin
		trigger <= 1;
		end 
							
	
	
					
							
	if (T==20000000) 
		begin
		sweep_end_flag <= 1;
		T<=0;
		E<=E+1'b1;
		e_number<=E;//test E for DR_CTL trouble shoot
		end
	if (T<20000000) 
		begin
		sweep_end_flag <=0;
		end
		
	if (e_flag !=0)
		begin	
		G<=G+1'b1;
		
		end
		
	if (G>=10'b1111111111)
		begin
		e_flag<=0;
		
		G<=0;
		final_massive_stop<=1;
		T<=0;
		
		end
		
	if (reset_flag !=0)
		begin
		E<=0;
		final_massive_stop<=0;
		T<=0;
		end
	
	//after whole sweep routine it can start over
	if (complete_end_flag !=0)
		begin
		e_flag<=1;
		N<=0;
		end
										
	
		
	
		
		
	end	
		
		
		
	
always@ (negedge ten_MHz_ext)	
	
	begin
	
	


	if ((T>=423) && (T<424))
			begin
if (E<=18)
	begin
	if (E<=17)
		begin
		if (E<=16)
			begin
			if (E<=15)
				begin
				if (E<=14)
					begin
					if (E<=13)
						begin
						if (E<=12)
							begin
							if (E<=11)
								begin
								if (E<=10)
									begin
									if (E<=9)
										begin
										if (E<=8)
											begin
											if (E<=7)
												begin
												if (E<=6)
													begin
													if (E<=5)
														begin
														if (E<=4)
															begin
																if (E<=3)
																	begin
																	if (E<=2)
																		begin
																		if (E<=1)
																			begin
																			if (E<=0)
																				begin
																				if (memory_reg[8:39] > memory_reg[40:71])
																					begin
																					DR_CTL<=0;
																					end
																				if (memory_reg[8:39] < memory_reg[40:71])
																					begin
																					DR_CTL<=1;
																					end	
																				
																				end
																			else
																				begin
																				
																				if (memory_reg[184+8:184+39] > memory_reg[184+40:184+71])
																					begin
																					DR_CTL<=0;
																					end
																				if (memory_reg[184+8:184+39] < memory_reg[184+40:184+71])
																					begin 
																					DR_CTL<=1;
																					end
																				end
																			end
																	else
																		begin
																		
																		if (memory_reg[184*2+8:184*2+39] > memory_reg[184*2+40:184*2+71])
																			begin
																			DR_CTL<=0;	
																			end
																		if (memory_reg[184*2+8:184*2+39] < memory_reg[184*2+40:184*2+71])
																			begin 
																			DR_CTL<=1;
																			end
																		end	
																	end
																else
																	begin
																	
																	if (memory_reg[184*3+8:184*3+39] > memory_reg[184*3+40:184*3+71])
																		begin
																		DR_CTL<=0;
																		end
																	if (memory_reg[184*3+8:184*3+39] < memory_reg[184*3+40:184*3+71])
																		begin 
																		DR_CTL<=1;
																		end
																	end	
																end 
															else
																begin
																
																if (memory_reg[184*4+8:184*4+39] > memory_reg[184*4+40:184*4+71])
																	begin
																	DR_CTL<=0;
																	end
																if (memory_reg[184*4+8:184*4+39] < memory_reg[184*4+40:184*4+71])
																	begin 
																	DR_CTL<=1;
																	end
																end		
															end							
														else
															begin
															
															if (memory_reg[184*5+8:184*5+39] > memory_reg[184*5+40:184*5+71])
																begin
																DR_CTL<=0;
																end
															if (memory_reg[184*5+8:184*5+39] < memory_reg[184*5+40:184*5+71])
																begin 
																DR_CTL<=1;
																end
															end	
														end
													else
														begin
														
														if (memory_reg[184*6+8:184*6+39] > memory_reg[184*6+40:184*6+71])
															begin
															DR_CTL<=0;
															end
														if (memory_reg[184*6+8:184*6+39] < memory_reg[184*6+40:184*6+71])
															begin 
															DR_CTL<=1;
															end
														end	
													end
												else
													begin
													
													if (memory_reg[184*7+8:184*7+39] > memory_reg[184*7+40:184*7+71])
														begin
														DR_CTL<=0;
														end
													if (memory_reg[184*7+8:184*7+39] < memory_reg[184*7+40:184*7+71])
														begin 
														DR_CTL<=1;
														end
													end	
												end
											else
												begin
												
												if (memory_reg[184*8+8:184*8+39] > memory_reg[184*8+40:184*8+71])
													begin
													DR_CTL<=0;
													end
												if (memory_reg[184*8+8:184*8+39] < memory_reg[184*8+40:184*8+71])
													begin 
													DR_CTL<=1;
													end
												end	
											end
										else
											begin
											
											if (memory_reg[184*9+8:184*9+39] > memory_reg[184*9+40:184*9+71])
												begin
												DR_CTL<=0;
												end
											if (memory_reg[184*9+8:184*9+39] < memory_reg[184*9+40:184*9+71])
												begin 
												DR_CTL<=1;
												end
											end	
										end
									else
										begin
										
										if (memory_reg[184*10+8:184*10+39] > memory_reg[184*10+40:184*10+71])
											begin
											DR_CTL<=0;
											end	
										if (memory_reg[184*10+8:184*10+39] < memory_reg[184*10+40:184*10+71])
											begin 
											DR_CTL<=1;
											end
										end	
									end
								else
									begin
									
									if (memory_reg[184*11+8:184*11+39] > memory_reg[184*11+40:184*11+71])
										begin
										DR_CTL<=0;
										end
									if (memory_reg[184*11+8:184*11+39] < memory_reg[184*11+40:184*11+71])
										begin 
										DR_CTL<=1;
										end
									end		
								end
							else
								begin
								
								if (memory_reg[184*12+8:184*12+39] > memory_reg[184*12+40:184*12+71])
									begin
									DR_CTL<=0;
									end
								if (memory_reg[184*12+8:184*12+39] < memory_reg[184*12+40:184*12+71])
									begin 
									DR_CTL<=1;
									end
								end		
							end
						else
							begin
							
							if (memory_reg[184*13+8:184*13+39] > memory_reg[184*13+40:184*13+71])
								begin
								DR_CTL<=0;
								end
							if (memory_reg[184*13+8:184*13+39] < memory_reg[184*13+40:184*13+71])
								begin 
								DR_CTL<=1;
								end
							end		
						end 
					else
						begin
						
						if (memory_reg[184*14+8:184*14+39] > memory_reg[184*14+40:184*14+71])
							begin
							DR_CTL<=0;
							end
						if (memory_reg[184*14+8:184*14+39] < memory_reg[184*14+40:184*14+71])
							begin 
							DR_CTL<=1;
							end
						end		
					end							
				else
					begin
					
					if (memory_reg[184*15+8:184*15+39] > memory_reg[184*15+40:184*15+71])
						begin
						DR_CTL<=0;
						end
					if (memory_reg[184*15+8:184*15+39] < memory_reg[184*15+40:184*15+71])
						begin 
						DR_CTL<=1;
						end
					end	
				end
			else
				begin
				
				if (memory_reg[184*16+8:184*16+39] > memory_reg[184*16+40:184*16+71])
					begin
					DR_CTL<=0;
					end
				if (memory_reg[184*16+8:184*16+39] < memory_reg[184*16+40:184*16+71])
					begin 
					DR_CTL<=1;
					end
				end	
			end
		else
			begin
			
			if (memory_reg[184*17+8:184*17+39] > memory_reg[184*17+40:184*17+71])
				begin
				DR_CTL<=0;
				end
			if (memory_reg[184*17+8:184*17+39] < memory_reg[184*17+40:184*17+71])
				begin 			
				DR_CTL<=1;
				end
			end	
		end
	else
		begin
		
		if (memory_reg[184*18+8:184*18+39] > memory_reg[184*18+40:184*18+71])
			begin
			DR_CTL<=0;
			end
		if (memory_reg[184*18+8:184*18+39] < memory_reg[184*18+40:184*18+71])
			begin 
			DR_CTL<=1;
			end
		end	
	end
else
	begin
	
	if (memory_reg[184*19+8:184*19+39] > memory_reg[184*19+40:184*19+71])
		begin
		DR_CTL<=0;
		end
	if (memory_reg[184*19+8:184*19+39] < memory_reg[184*19+40:184*19+71])
		begin 
		DR_CTL<=1; 
		end
	end	
	
		end
		

	if (T>=424)
		begin
if (E<=18)
	begin
	if (E<=17)
		begin
		if (E<=16)
			begin
			if (E<=15)
				begin
				if (E<=14)
					begin
					if (E<=13)
						begin
						if (E<=12)
							begin
							if (E<=11)
								begin
								if (E<=10)
									begin
									if (E<=9)
										begin
										if (E<=8)
											begin
											if (E<=7)
												begin
												if (E<=6)
													begin
													if (E<=5)
														begin
														if (E<=4)
															begin
															if (E<=3)
																	begin
																	if (E<=2)
																		begin
																		if (E<=1)
																			begin
																			if (E<=0)
																				begin
																				if (memory_reg[8:39] > memory_reg[40:71])
																					begin
																					DR_CTL<=1;
																					end
																				if (memory_reg[8:39] < memory_reg[40:71])
																					begin
																					DR_CTL<=0;
																					end	
																				
																				end
																			else
																				begin
																				
																				if (memory_reg[184+8:184+39] > memory_reg[184+40:184+71])
																					begin
																					DR_CTL<=1;
																					end
																				if (memory_reg[184+8:184+39] < memory_reg[184+40:184+71])
																					begin 
																					DR_CTL<=0;
																					end
																				end
																			end
																	else
																		begin
																		if (memory_reg[184*2+8:184*2+39] > memory_reg[184*2+40:184*2+71])
																			begin
																			DR_CTL<=1;
																			end
																		if (memory_reg[184*2+8:184*2+39] < memory_reg[184*2+40:184*2+71])
																			begin
																			DR_CTL<=0;
																			end	
																
																		end	
																	end
																else
																	begin
																	if (memory_reg[184*3+8:184*3+39] > memory_reg[184*3+40:184*3+71])
																		begin
																		DR_CTL<=1;
																		end
																	if (memory_reg[184*3+8:184*3+39] < memory_reg[184*3+40:184*3+71])
																		begin
																		DR_CTL<=0;
																		end	
																
																	end	
																end 
															else
																begin
																if (memory_reg[184*4+8:184*4+39] > memory_reg[184*4+40:184*4+71])
																	begin
																	DR_CTL<=1;
																	end
																if (memory_reg[184*4+8:184*4+39] < memory_reg[184*4+40:184*4+71])
																	begin
																	DR_CTL<=0;
																	end	
																
																end		
															end							
														else
															begin
															if (memory_reg[184*5+8:184*5+39] > memory_reg[184*5+40:184*5+71])
																begin
																DR_CTL<=1;
																end
															if (memory_reg[184*5+8:184*5+39] < memory_reg[184*5+40:184*5+71])
																begin
																DR_CTL<=0;
																end	
															
															end	
														end
													else
														begin
														if (memory_reg[184*6+8:184*6+39] > memory_reg[184*6+40:184*6+71])
															begin
															DR_CTL<=1;
															end
														if (memory_reg[184*6+8:184*6+39] < memory_reg[184*6+40:184*6+71])
															begin
															DR_CTL<=0;
															end	
													
														end	
													end
												else
													begin
													if (memory_reg[184*7+8:184*7+39] > memory_reg[184*7+40:184*7+71])
														begin
														DR_CTL<=1;
														end
													if (memory_reg[184*7+8:184*7+39] < memory_reg[184*7+40:184*7+71])
														begin
														DR_CTL<=0;
														end	
													
													end	
												end
											else
												begin
												if (memory_reg[184*8+8:184*8+39] > memory_reg[184*8+40:184*8+71])
													begin
													DR_CTL<=1;
													end
												if (memory_reg[184*8+8:184*8+39] < memory_reg[184*8+40:184*8+71])
													begin
													DR_CTL<=0;
													end	
												
												end	
											end
										else
											begin
											if (memory_reg[184*9+8:184*9+39] > memory_reg[184*9+40:184*9+71])
												begin
												DR_CTL<=1;
												end
											if (memory_reg[184*9+8:184*9+39] < memory_reg[184*9+40:184*9+71])
												begin
												DR_CTL<=0;
												end	
											
											end	
										end
									else
										begin
										if (memory_reg[184*10+8:184*10+39] > memory_reg[184*10+40:184*10+71])
											begin
											DR_CTL<=1;
											end
										if (memory_reg[184*10+8:184*10+39] < memory_reg[184*10+40:184*10+71])
											begin
											DR_CTL<=0;
											end	
										
										end	
									end
								else
									begin
									if (memory_reg[184*11+8:184*11+39] > memory_reg[184*11+40:184*11+71])
										begin
										DR_CTL<=1;
										end
									if (memory_reg[184*11+8:184*11+39] < memory_reg[184*11+40:184*11+71])
										begin
										DR_CTL<=0;
										end	
									
									end		
								end
							else
								begin
								if (memory_reg[184*12+8:184*12+39] > memory_reg[184*12+40:184*12+71])
									begin
									DR_CTL<=1;
									end
								if (memory_reg[184*12+8:184*12+39] < memory_reg[184*12+40:184*12+71])
									begin
									DR_CTL<=0;
									end	
								
								end		
							end
						else
							begin
							if (memory_reg[184*13+8:184*13+39] > memory_reg[184*13+40:184*13+71])
								begin
								DR_CTL<=1;
								end
							if (memory_reg[184*13+8:184*13+39] < memory_reg[184*13+40:184*13+71])
								begin
								DR_CTL<=0;
								end	
							
							end		
						end 
					else
						begin
						if (memory_reg[184*14+8:184*14+39] > memory_reg[184*14+40:184*14+71])
							begin
							DR_CTL<=1;
							end
						if (memory_reg[184*14+8:184*14+39] < memory_reg[184*14+40:184*14+71])
							begin
							DR_CTL<=0;
							end	
					
						end		
					end							
				else
					begin
					if (memory_reg[184*15+8:184*15+39] > memory_reg[184*15+40:184*15+71])
						begin
						DR_CTL<=1;
						end
					if (memory_reg[184*15+8:184*15+39] < memory_reg[184*15+40:184*15+71])
						begin
						DR_CTL<=0;
						end	
										
					end	
				end
			else
				begin
				if (memory_reg[184*16+8:184*16+39] > memory_reg[184*16+40:184*16+71])
					begin
					DR_CTL<=1;
					end
				if (memory_reg[184*16+8:184*16+39] < memory_reg[184*16+40:184*16+71])
					begin
					DR_CTL<=0;
					end	
				
				end	
			end
		else
			begin
			if (memory_reg[184*17+8:184*17+39] > memory_reg[184*17+40:184*17+71])
				begin
				DR_CTL<=1;
				end
			if (memory_reg[184*17+8:184*17+39] < memory_reg[184*17+40:184*17+71])
				begin
				DR_CTL<=0;
				end	
			
			end	
		end
	else
		begin
		if (memory_reg[184*18+8:184*18+39] > memory_reg[184*18+40:184*18+71])
			begin
			DR_CTL<=1;
			end
		if (memory_reg[184*18+8:184*18+39] < memory_reg[184*18+40:184*18+71])
			begin
			DR_CTL<=0;
			end	
		
		end	
	end
else
	begin
	if (memory_reg[184*19+8:184*19+39] > memory_reg[184*19+40:184*19+71])
		begin
		DR_CTL<=1;
		end
	if (memory_reg[184*19+8:184*19+39] < memory_reg[184*19+40:184*19+71])
		begin
		DR_CTL<=0;
		end	
	
	end	
		end		
	
	/*
	
///this code gives the correct DR_CTL sequence based on whether the sweep is up or down
//it compares the appropriate places in the larger memory register within each 184_bit section and then this section sets the 
//DR_CTL to its state after the toggle  
// low to high sweeps occur when the first part of the step size register is higher than the second
//the lowering step size is the highest  "memory_reg[184*n + 8:  184*n + 39] > memory_reg[184*n + 40: 184*n + 71]" 
//where DR_CTL is set.  so from hi to low, 8:39<40:71, after toggle Dr_CTL is cleared -- DR_DTL to start high and go low.	
	
	
	
	if ((T>=422) && (T<423))
			begin
if (E<=18)
	begin
	if (E<=17)
		begin
		if (E<=16)
			begin
			if (E<=15)
				begin
				if (E<=14)
					begin
					if (E<=13)
						begin
						if (E<=12)
							begin
							if (E<=11)
								begin
								if (E<=10)
									begin
									if (E<=9)
										begin
										if (E<=8)
											begin
											if (E<=7)
												begin
												if (E<=6)
													begin
													if (E<=5)
														begin
														if (E<=4)
															begin
																if (E<=3)
																	begin
																	if (E<=2)
																		begin
																		if (E<=1)
																			begin
																			if (E<=0)
																				begin
																				if (memory_reg[8:39] > memory_reg[40:71])
																					begin
																					DR_CTL<=0;
																					end
																				if (memory_reg[8:39] < memory_reg[40:71])
																					begin
																					DR_CTL<=1;
																					end	
																				
																				end
																			else
																				begin
																				
																				if (memory_reg[184+8:184+39] > memory_reg[184+40:184+71])
																					begin
																					DR_CTL<=0;
																					end
																				if (memory_reg[184+8:184+39] < memory_reg[184+40:184+71])
																					begin 
																					DR_CTL<=1;
																					end
																				end
																			end
																	else
																		begin
																		
																		if (memory_reg[184*2+8:184*2+39] > memory_reg[184*2+40:184*2+71])
																			begin
																			DR_CTL<=0;	
																			end
																		if (memory_reg[184*2+8:184*2+39] < memory_reg[184*2+40:184*2+71])
																			begin 
																			DR_CTL<=1;
																			end
																		end	
																	end
																else
																	begin
																	
																	if (memory_reg[184*3+8:184*3+39] > memory_reg[184*3+40:184*3+71])
																		begin
																		DR_CTL<=0;
																		end
																	if (memory_reg[184*3+8:184*3+39] < memory_reg[184*3+40:184*3+71])
																		begin 
																		DR_CTL<=1;
																		end
																	end	
																end 
															else
																begin
																
																if (memory_reg[184*4+8:184*4+39] > memory_reg[184*4+40:184*4+71])
																	begin
																	DR_CTL<=0;
																	end
																if (memory_reg[184*4+8:184*4+39] < memory_reg[184*4+40:184*4+71])
																	begin 
																	DR_CTL<=1;
																	end
																end		
															end							
														else
															begin
															
															if (memory_reg[184*5+8:184*5+39] > memory_reg[184*5+40:184*5+71])
																begin
																DR_CTL<=0;
																end
															if (memory_reg[184*5+8:184*5+39] < memory_reg[184*5+40:184*5+71])
																begin 
																DR_CTL<=1;
																end
															end	
														end
													else
														begin
														
														if (memory_reg[184*6+8:184*6+39] > memory_reg[184*6+40:184*6+71])
															begin
															DR_CTL<=0;
															end
														if (memory_reg[184*6+8:184*6+39] < memory_reg[184*6+40:184*6+71])
															begin 
															DR_CTL<=1;
															end
														end	
													end
												else
													begin
													
													if (memory_reg[184*7+8:184*7+39] > memory_reg[184*7+40:184*7+71])
														begin
														DR_CTL<=0;
														end
													if (memory_reg[184*7+8:184*7+39] < memory_reg[184*7+40:184*7+71])
														begin 
														DR_CTL<=1;
														end
													end	
												end
											else
												begin
												
												if (memory_reg[184*8+8:184*8+39] > memory_reg[184*8+40:184*8+71])
													begin
													DR_CTL<=0;
													end
												if (memory_reg[184*8+8:184*8+39] < memory_reg[184*8+40:184*8+71])
													begin 
													DR_CTL<=1;
													end
												end	
											end
										else
											begin
											
											if (memory_reg[184*9+8:184*9+39] > memory_reg[184*9+40:184*9+71])
												begin
												DR_CTL<=0;
												end
											if (memory_reg[184*9+8:184*9+39] < memory_reg[184*9+40:184*9+71])
												begin 
												DR_CTL<=1;
												end
											end	
										end
									else
										begin
										
										if (memory_reg[184*10+8:184*10+39] > memory_reg[184*10+40:184*10+71])
											begin
											DR_CTL<=0;
											end	
										if (memory_reg[184*10+8:184*10+39] < memory_reg[184*10+40:184*10+71])
											begin 
											DR_CTL<=1;
											end
										end	
									end
								else
									begin
									
									if (memory_reg[184*11+8:184*11+39] > memory_reg[184*11+40:184*11+71])
										begin
										DR_CTL<=0;
										end
									if (memory_reg[184*11+8:184*11+39] < memory_reg[184*11+40:184*11+71])
										begin 
										DR_CTL<=1;
										end
									end		
								end
							else
								begin
								
								if (memory_reg[184*12+8:184*12+39] > memory_reg[184*12+40:184*12+71])
									begin
									DR_CTL<=0;
									end
								if (memory_reg[184*12+8:184*12+39] < memory_reg[184*12+40:184*12+71])
									begin 
									DR_CTL<=1;
									end
								end		
							end
						else
							begin
							
							if (memory_reg[184*13+8:184*13+39] > memory_reg[184*13+40:184*13+71])
								begin
								DR_CTL<=0;
								end
							if (memory_reg[184*13+8:184*13+39] < memory_reg[184*13+40:184*13+71])
								begin 
								DR_CTL<=1;
								end
							end		
						end 
					else
						begin
						
						if (memory_reg[184*14+8:184*14+39] > memory_reg[184*14+40:184*14+71])
							begin
							DR_CTL<=0;
							end
						if (memory_reg[184*14+8:184*14+39] < memory_reg[184*14+40:184*14+71])
							begin 
							DR_CTL<=1;
							end
						end		
					end							
				else
					begin
					
					if (memory_reg[184*15+8:184*15+39] > memory_reg[184*15+40:184*15+71])
						begin
						DR_CTL<=0;
						end
					if (memory_reg[184*15+8:184*15+39] < memory_reg[184*15+40:184*15+71])
						begin 
						DR_CTL<=1;
						end
					end	
				end
			else
				begin
				
				if (memory_reg[184*16+8:184*16+39] > memory_reg[184*16+40:184*16+71])
					begin
					DR_CTL<=0;
					end
				if (memory_reg[184*16+8:184*16+39] < memory_reg[184*16+40:184*16+71])
					begin 
					DR_CTL<=1;
					end
				end	
			end
		else
			begin
			
			if (memory_reg[184*17+8:184*17+39] > memory_reg[184*17+40:184*17+71])
				begin
				DR_CTL<=0;
				end
			if (memory_reg[184*17+8:184*17+39] < memory_reg[184*17+40:184*17+71])
				begin 			
				DR_CTL<=1;
				end
			end	
		end
	else
		begin
		
		if (memory_reg[184*18+8:184*18+39] > memory_reg[184*18+40:184*18+71])
			begin
			DR_CTL<=0;
			end
		if (memory_reg[184*18+8:184*18+39] < memory_reg[184*18+40:184*18+71])
			begin 
			DR_CTL<=1;
			end
		end	
	end
else
	begin
	
	if (memory_reg[184*19+8:184*19+39] > memory_reg[184*19+40:184*19+71])
		begin
		DR_CTL<=0;
		end
	if (memory_reg[184*19+8:184*19+39] < memory_reg[184*19+40:184*19+71])
		begin 
		DR_CTL<=1; 
		end
	end	
		end



///this code gives the correct DR_CTL sequence based on whether the sweep is up or down
//it compares the appropriate places in the larger memory register within each 184_bit section and then this section sets the 
//DR_CTL to its state after the toggle  
// low to high sweeps occur when the first part of the step size register is higher than the second
//the lowering step size is the highest  "memory_reg[184*n + 8:  184*n + 39] > memory_reg[184*n + 40: 184*n + 71]" 
//where DR_CTL is set.  so from hi to low, 8:39<40:71, after toggle Dr_CTL is cleared -- DR_DTL to start high and go low.




	if (T>=423)
		begin
if (E<=18)
	begin
	if (E<=17)
		begin
		if (E<=16)
			begin
			if (E<=15)
				begin
				if (E<=14)
					begin
					if (E<=13)
						begin
						if (E<=12)
							begin
							if (E<=11)
								begin
								if (E<=10)
									begin
									if (E<=9)
										begin
										if (E<=8)
											begin
											if (E<=7)
												begin
												if (E<=6)
													begin
													if (E<=5)
														begin
														if (E<=4)
															begin
															if (E<=3)
																	begin
																	if (E<=2)
																		begin
																		if (E<=1)
																			begin
																			if (E<=0)
																				begin
																				if (memory_reg[8:39] > memory_reg[40:71])
																					begin
																					DR_CTL<=1;
																					end
																				if (memory_reg[8:39] < memory_reg[40:71])
																					begin
																					DR_CTL<=0;
																					end	
																				
																				end
																			else
																				begin
																				
																				if (memory_reg[184+8:184+39] > memory_reg[184+40:184+71])
																					begin
																					DR_CTL<=1;
																					end
																				if (memory_reg[184+8:184+39] < memory_reg[184+40:184+71])
																					begin 
																					DR_CTL<=0;
																					end
																				end
																			end
																	else
																		begin
																		if (memory_reg[184*2+8:184*2+39] > memory_reg[184*2+40:184*2+71])
																			begin
																			DR_CTL<=1;
																			end
																		if (memory_reg[184*2+8:184*2+39] < memory_reg[184*2+40:184*2+71])
																			begin
																			DR_CTL<=0;
																			end	
																
																		end	
																	end
																else
																	begin
																	if (memory_reg[184*3+8:184*3+39] > memory_reg[184*3+40:184*3+71])
																		begin
																		DR_CTL<=1;
																		end
																	if (memory_reg[184*3+8:184*3+39] < memory_reg[184*3+40:184*3+71])
																		begin
																		DR_CTL<=0;
																		end	
																
																	end	
																end 
															else
																begin
																if (memory_reg[184*4+8:184*4+39] > memory_reg[184*4+40:184*4+71])
																	begin
																	DR_CTL<=1;
																	end
																if (memory_reg[184*4+8:184*4+39] < memory_reg[184*4+40:184*4+71])
																	begin
																	DR_CTL<=0;
																	end	
																
																end		
															end							
														else
															begin
															if (memory_reg[184*5+8:184*5+39] > memory_reg[184*5+40:184*5+71])
																begin
																DR_CTL<=1;
																end
															if (memory_reg[184*5+8:184*5+39] < memory_reg[184*5+40:184*5+71])
																begin
																DR_CTL<=0;
																end	
															
															end	
														end
													else
														begin
														if (memory_reg[184*6+8:184*6+39] > memory_reg[184*6+40:184*6+71])
															begin
															DR_CTL<=1;
															end
														if (memory_reg[184*6+8:184*6+39] < memory_reg[184*6+40:184*6+71])
															begin
															DR_CTL<=0;
															end	
													
														end	
													end
												else
													begin
													if (memory_reg[184*7+8:184*7+39] > memory_reg[184*7+40:184*7+71])
														begin
														DR_CTL<=1;
														end
													if (memory_reg[184*7+8:184*7+39] < memory_reg[184*7+40:184*7+71])
														begin
														DR_CTL<=0;
														end	
													
													end	
												end
											else
												begin
												if (memory_reg[184*8+8:184*8+39] > memory_reg[184*8+40:184*8+71])
													begin
													DR_CTL<=1;
													end
												if (memory_reg[184*8+8:184*8+39] < memory_reg[184*8+40:184*8+71])
													begin
													DR_CTL<=0;
													end	
												
												end	
											end
										else
											begin
											if (memory_reg[184*9+8:184*9+39] > memory_reg[184*9+40:184*9+71])
												begin
												DR_CTL<=1;
												end
											if (memory_reg[184*9+8:184*9+39] < memory_reg[184*9+40:184*9+71])
												begin
												DR_CTL<=0;
												end	
											
											end	
										end
									else
										begin
										if (memory_reg[184*10+8:184*10+39] > memory_reg[184*10+40:184*10+71])
											begin
											DR_CTL<=1;
											end
										if (memory_reg[184*10+8:184*10+39] < memory_reg[184*10+40:184*10+71])
											begin
											DR_CTL<=0;
											end	
										
										end	
									end
								else
									begin
									if (memory_reg[184*11+8:184*11+39] > memory_reg[184*11+40:184*11+71])
										begin
										DR_CTL<=1;
										end
									if (memory_reg[184*11+8:184*11+39] < memory_reg[184*11+40:184*11+71])
										begin
										DR_CTL<=0;
										end	
									
									end		
								end
							else
								begin
								if (memory_reg[184*12+8:184*12+39] > memory_reg[184*12+40:184*12+71])
									begin
									DR_CTL<=1;
									end
								if (memory_reg[184*12+8:184*12+39] < memory_reg[184*12+40:184*12+71])
									begin
									DR_CTL<=0;
									end	
								
								end		
							end
						else
							begin
							if (memory_reg[184*13+8:184*13+39] > memory_reg[184*13+40:184*13+71])
								begin
								DR_CTL<=1;
								end
							if (memory_reg[184*13+8:184*13+39] < memory_reg[184*13+40:184*13+71])
								begin
								DR_CTL<=0;
								end	
							
							end		
						end 
					else
						begin
						if (memory_reg[184*14+8:184*14+39] > memory_reg[184*14+40:184*14+71])
							begin
							DR_CTL<=1;
							end
						if (memory_reg[184*14+8:184*14+39] < memory_reg[184*14+40:184*14+71])
							begin
							DR_CTL<=0;
							end	
					
						end		
					end							
				else
					begin
					if (memory_reg[184*15+8:184*15+39] > memory_reg[184*15+40:184*15+71])
						begin
						DR_CTL<=1;
						end
					if (memory_reg[184*15+8:184*15+39] < memory_reg[184*15+40:184*15+71])
						begin
						DR_CTL<=0;
						end	
										
					end	
				end
			else
				begin
				if (memory_reg[184*16+8:184*16+39] > memory_reg[184*16+40:184*16+71])
					begin
					DR_CTL<=1;
					end
				if (memory_reg[184*16+8:184*16+39] < memory_reg[184*16+40:184*16+71])
					begin
					DR_CTL<=0;
					end	
				
				end	
			end
		else
			begin
			if (memory_reg[184*17+8:184*17+39] > memory_reg[184*17+40:184*17+71])
				begin
				DR_CTL<=1;
				end
			if (memory_reg[184*17+8:184*17+39] < memory_reg[184*17+40:184*17+71])
				begin
				DR_CTL<=0;
				end	
			
			end	
		end
	else
		begin
		if (memory_reg[184*18+8:184*18+39] > memory_reg[184*18+40:184*18+71])
			begin
			DR_CTL<=1;
			end
		if (memory_reg[184*18+8:184*18+39] < memory_reg[184*18+40:184*18+71])
			begin
			DR_CTL<=0;
			end	
		
		end	
	end
else
	begin
	if (memory_reg[184*19+8:184*19+39] > memory_reg[184*19+40:184*19+71])
		begin
		DR_CTL<=1;
		end
	if (memory_reg[184*19+8:184*19+39] < memory_reg[184*19+40:184*19+71])
		begin
		DR_CTL<=0;
		end	
	
	end	
		end
		*/
	
	
									
								
	end									
										
										
										

		
		
		
//this section of code deals with reading the message from the rabbit
always@ (posedge SCLK_PE_3)
	begin
	
	if (reset_flag !=0)
		begin
		i<=0;
		
		memory_reg<= 3680'b0;
		end
	
			
			
	
	if (dont_read_flag == 0)			
		begin
		memory_reg[i] <= SDIO_PE_5;
		i<=i+1;
		end
	end
		

			
endmodule			
	