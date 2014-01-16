module Rabbit_2_FPGA_2_DDS_multi_FIFO(


input key_3_sweep;
input tenMHz_ext;
input SDIO_PE_5_1;
input SCLK_PE_3_1;

output reg SDIO;
output reg SCLK;
output reg IO_UPDATE;
output reg DR_CTL;




integer i;  //index for the memory as it is being read in
integer N;  // index for the memory being read out
integer M; //tells when to send out the SDIO signal
integer L; //tells when to toggle the SCLK signal




reg sweep_key_flag; // this is high during the time that the sweep info is being sent out to the DDS
					//it must be reset every time that each signal has been sent 
					

reg sweep_end_flag;//this flag is: when it is set sweep_key_flag can be set back to zero


reg complete_end_flag;  //this flag will occur when the whole memory_reg has been sent (when N>=i)
						//it resets the N and i indices to zero so the thing can be restarted
						
reg [0:3679] memory_reg;
					
					
					
					
					
					


//this section of code deals with making the decisions about when to start doing what
//its main job is to make sure the FPGA knows when to send the signal to the DDS
// and when to reset everything
always@ (negedge tenMHz_ext)
	begin



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





	//this section will set the complete_end_flag which will indicate that the whole set of sweep has been run
	//this happens when N>=i this will reset everything important including setting N and i back to Zero
	if (N>=i)
		begin
		complete_end_flag <=1;
		end
	if (N<i)
		begin
		complete_end_flag<=0;
		end
	






	end






//this section of code deals with sending out the signal to the DDS 
always@ (posedge tenMHz_ext)
	begin
	if (complete_end_flag !=0)
		begin
		N<=0;
		end





	//when sweep_end_flag is high sweep_key_flag can be reset the number mus be so high because the trigger from the keys can be >1sec
	
	if (T>=20000000) 
		begin
		sweep_end_flag <= 1;
		T<=0;
		E<=E+1;
		end
	if (T<20000000) 
		begin
		sweep_end_flag <=0;
		end
		





	if (sweep_key_flag != 0)
		begin
		
		
		//T increments while sweep_key_flag is high. when T >=20,000,000 sweep_key_flag will go back to zero
		T<=T+1;
		
		end
		
		
		
		
		
		
		
		
	//this is the section of code that toggles the IO_UPDATE
	if ((T>= 422) && (T<427))
		begin
		IO_UPDATE <= 1;
		end
	if ((T<422) || (T>=427))
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
			L<=L+1;
			SCLK=0;
			end
		if (M < 1)
			begin
			M<=M+1;
				
			//the main difference with this program to have multiple sweeps
			//is that N is not reset after every sweep
			SDIO <= memory_reg[N];
			end
				
		if (M >= 1)
			begin
			N<= N+1;
			M<=0;
			end
		end
	if ((T<5) || (T>=373))
		begin
		SCLK<=0;
		SDIO<=0;
		end
			
		
		
		
		
		
		
		
		
		
		
		
		
	//this section of code toggles SCLK in the appropriate manner	
	if ((T>=446) && (T<451))
			begin
if (P=<18)
	begin
	if (P=<17)
		begin
		if (P=<16)
			begin
			if (P=<15)
				begin
				if (P=<14)
					begin
					if (P=<13)
						begin
						if (P=<12)
							begin
							if (P=<11)
								begin
								if (P=<10)
									begin
									if (P=<9)
										begin
										if (P=<8)
											begin
											if (P=<7)
												begin
												if (P=<6)
													begin
													if (P=<5)
														begin
														if (P=<4)
															begin
															if (P=<5)
																begin
																if (P=<3)
																	begin
																	if (P=<2)
																		begin
																		if (P=<1)
																			begin
																			if (P=<0)
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
											
											if (memory_reg[184*9+8:184*2+39] > memory_reg[184*9+40:184*2+71])
												begin
												DR_CTL<=0;
												end
											if (memory_reg[184*9+8:184*2+39] < memory_reg[184*9+40:184*2+71])
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
									
									if (memory_reg[184*11+8:184*10+39] > memory_reg[184*11+40:184*10+71])
										begin
										DR_CTL<=0;
										end
									if (memory_reg[184*11+8:184*10+39] < memory_reg[184*11+40:184*10+71])
										begin 
										DR_CTL<=1;
										end
									end		
								end
							else
								begin
								
								if (memory_reg[184*12+8:184*2+39] > memory_reg[184*12+40:184*2+71])
									begin
									DR_CTL<=0;
									end
								if (memory_reg[184*12+8:184*2+39] < memory_reg[184*12+40:184*2+71])
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









	if ((T<446) || (T>=451))
		begin
if (P=<18)
	begin
	if (P=<17)
		begin
		if (P=<16)
			begin
			if (P=<15)
				begin
				if (P=<14)
					begin
					if (P=<13)
						begin
						if (P=<12)
							begin
							if (P=<11)
								begin
								if (P=<10)
									begin
									if (P=<9)
										begin
										if (P=<8)
											begin
											if (P=<7)
												begin
												if (P=<6)
													begin
													if (P=<5)
														begin
														if (P=<4)
															begin
															if (P=<5)
																begin
																if (P=<3)
																	begin
																	if (P=<2)
																		begin
																		if (P=<1)
																			begin
																			if (P=<0)
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
												if (memory_reg[184*8+8:184*8+39] > memory_reg[184*8+40:184*8+71])
													begin
													DR_CTL<=0;
													end	
												
												end	
											end
										else
											begin
											if (memory_reg[184*9+8:184*2+39] > memory_reg[184*9+40:184*2+71])
												begin
												DR_CTL<=1;
												end
											if (memory_reg[184*9+8:184*2+39] < memory_reg[184*9+40:184*2+71])
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
									if (memory_reg[184*11+8:184*10+39] > memory_reg[184*11+40:184*10+71])
										begin
										DR_CTL<=1;
										end
									if (memory_reg[184*11+8:184*10+39] < memory_reg[184*11+40:184*10+71])
										begin
										DR_CTL<=0;
										end	
									
									end		
								end
							else
								begin
								if (memory_reg[184*12+8:184*2+39] > memory_reg[184*12+40:184*2+71])
									begin
									DR_CTL<=1;
									end
								if (memory_reg[184*12+8:184*2+39] < memory_reg[184*12+40:184*2+71])
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
							
							
							
									
									
									
									
									
										
										
										
								
								
								
								
		
	
		
		

			
			
			
		
		
	end
		
		
		
		
		
//this section of code deals with reading the message from the rabbit
always@ (posedge SCLK_PE_3_1)
	begin
	if (complete_end_flag !=0)
		begin
		i<=0;
		end
			
			
			
	memory_reg[i] <= SDIO_PE_5_1;
				
	i<=i+1;
	end
		

			
endmodule			
	