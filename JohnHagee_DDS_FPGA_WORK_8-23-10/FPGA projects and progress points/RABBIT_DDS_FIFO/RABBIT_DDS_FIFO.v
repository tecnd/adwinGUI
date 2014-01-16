/**************








******************/


module RABBIT_DDS_FIFO(init_trigger, sweep_trig, ten_MHz_ext, RABBIT_DATA, RABBIT_CLOK, 
SDIO, SCLK, IO_UPDATE, DR_CTL, OSK, CSB, DR_HOLD, IO_RESET, IO_trigger_out,sweep_trig_flag, 
 sweep_routine_end_flag, E_stop_flag,end_routine_stop, reset_flag,  init_key_flag,dont_read_flag);



/***************









********************/

input sweep_trig;
input ten_MHz_ext;
input RABBIT_DATA;
input RABBIT_CLOK;

input init_trigger;


/****************
The following are all output pins on the FPGA.

The first group are all inputs to the DDS.


SDIO 		- 	This is the SERIAL DATA pin on the DDS.  This line recieves addresses of 
				registers of the DDS followed by a certain number of bits being expected 
				by the DDS to fill those registers.
		
SCLK 		-	This is the SERIAL CLOCK input for the DDS.  

IO_UPDATE  	- 	This is the pin that transfers the register information from the buffers 
				into the active registers.  Changes take place on this pins rising edge.









*******************/

//outputs to the DDS
output reg SDIO;
output reg SCLK;
output reg IO_UPDATE;
output reg DR_CTL;
output reg OSK;
output reg CSB;
output reg DR_HOLD;
output reg IO_RESET;



output reg IO_trigger_out;

/********
The following two parameters form the total initalize message being sent during the initialize stage
of the DDS operation.

The first parameter, "INIT_REG," 

	*sets the upper and lower frequency limits at address 0x0b (8'h0b) to zero (32'h00000000,32'h00000000)
	
	*sets CONTROL FUNCTION REGISTER ONE  at address 0x00 (8'h00) to enable the 
	 DIGITAL RAMP ACCUMULATOR CLEAR, CFR1[12] and sets the rest to defaults (32'h00001000)
	 CFR1 default: 32'h00000000
	
	*sets CONTROL FUNCTION REGISTER TWO at address 0x01 (8'h01) to enable DIGITAL RAMP MODE
	 CFR2[19] and clears the PARALLEL DATA CLOCK enable CFR2[11] and sets the rest to defaults 
	 (32'h00480020)
	 CFR2 default: 32'h00400820
	
	
	
	
	*sets CONTROL FUNCTION REGISTER THREE at address 0x02 (8'h02) 
	
	
	
	 Justin Winkler's explanation for these settings is in quotes. I carried the settings over 
	 without changing (Non-default settings ***):
	
	
		"Setting for control register 3 using multiplier"
		
	
		   CFR3[31:30] = 2'b00, open bits (don't cares)
		
		***CFR3[29:28] = 2'b11, "DRV0 set so that REFCLK_OUT outputs a high current"
		
   		   CFR3[27] = 1'b0, open bit (don't care)

		***CFR3[26:24] = 3'b101, "Use VCO5 for PLL output range of 820-1150 MHz"
		
		
		(8'b00110101 = 8'h35)
		
		
		   CFR3[23:16] = 8'h3f, default settings
		
		
		
		***CFR3[15] = 1'b1, "bypass input divider"
		
		   CFR3[14] = 1'b1, default
		   CFR3[13:11] = 3'b000, open bits (don't cares)
		   CFR3[10] = 1'b0, default
		   CFR3[9] = 1'b0, open bit (don't care)
		
		***CFR3[8] = 1'b1, "Enable PLL"
		
		
		(8'b11000001 = 8'hc1)
		
		
		
		***CFR3[7:1] = 7'b1100100 "Set N to 100" listed in manual as N[6:0]
		
		   CFR3[0] = 1'b0, open bit (don't care)
		
		
		(8'b11001000 = 8'hc8)
		
		
		
	 (32'h353fc1c8)
	 CFR3 default: 32'h1f3f4000
	
	
	
	
	*sets AUXILLARY DAC CONTROL at address 0x03 (8'h03)
	
	 
	 Justin Winkler's explanation for these settings is in quotes. I carried the settings over 
	 without changing (Non-default settings ***):
	
		"Set the full scale output current through this register. I'm setting this as low as possible."
		
	   
		   AUXILLARY DAC CONTROL REGISTER[31:8] = 24'h000000 open bits (don't cares)
		***AUXILLARY DAC CONTROL REGISTER[7:0]	= FSC[7:0] (Full Scale Current) = 8'h00
		
	 (32'h00000000)
	 AUXILLARY DAC CONTROL REGISTER default: 32'h0000007f
	


After IO_UPDATE is toggled these become the active settings for the DDS.
As per CFR1[12] being the DIGITAL RAMP ACCUMULATOR CLEAR which was set to high,
this IO_UPDATE will clear the DIGITAL RAMP ACCUMULATOR.



Now, the DIGITAL RAMP ACCUMULATOR CLEAR must not be enabled, so that the ramping can happen.
"UNCLEAR_REG"  consists of the address for CFR1, 0x00 (8'h00) and all of the CFR1 defaults,
including CLEAR DIGITAL RAMP ACCUMULATOR, CFR1[12] = 0.

UNCLEAR_REG = CFR1 address, 8'h00, and the default for CFR1 with bit 12 cleared, 32'h00000000

Once IO_UPDATE occurs after this message is sent, the DIGITAL RAMP ACCUMULATOR will be able to 
"accumulate."
			
		
	 
**********/


//The following parameter contains initializing settings and CLEAR DIGITAL RAMP ACCUMULATOR enable
parameter [0:231] INIT_REG = 	{8'h0b,32'h0,32'h0,8'h00,32'h00001000,8'h01,32'h00480020,
								8'h02,32'h353fc1c8,8'h03,32'h0};



//The following parameter disables CLEAR DIGITAL RAMP ACCUMULATOR so that ramps can run,
//conisting of CFR1 address and 32-bit word 32'h00000000 which is CFR1 default setting.
parameter [0:39] UNCLEAR_REG = {8'h00,32'h00000000};




/******
These are the counting registers.  Each is used for specific part of the program.  
They can be divided by function into time counters and index counters.

Time counters,

J,

T,  This is the main time counter for the sweeps being sent to the DDS.  The SDIO messages, IO_UPDATE,
	and DR_CTL are all manipulated according to this time counter.  It is incremeneted every TEN_MHz_ext
	clock cycle.
	
B,	This is the time counter for sending the initializing register data to the DDS.  SDIO messages,
	IO_UPDATE, and IO_RESET are manipulated according to this time counter while in the initialize stage.

G,

L,	This counter coordinates the SCLK and SDIO signals to send the 184-bit sweep information. It is only
	one bit.

M,	This counter coordinates the SCLK and SDIO signals to send the initislizing information.  It is only 
	one bit.




Index counters,

N, 	This holds the index number of the bit in "memory_reg" as it is being read out
	to the DDS.
	
i,	This holds the index number of the bit in "memory_reg" as it is being recieved
	from the RABBIT.
	
E,	This holds the index of sweeps read in and out of the FPGA -- important for
	the DR_CTL manipulation.
	
P,  This holds the index number when the parameters, "INIT_REG" and "UNCLEAR_REG"
	are being read out to the DDS to set up the initial settings for the DDS.


******/






//Time Counters

reg [24:0] T;	//time counter for sweeps
reg L; 			//tells when to toggle the SCLK signal

reg [10:0] B;	//time counter for initialize
reg M; 			//tells when to send out the SDIO signal

reg [5:0] G;	//counter to time complete reset
reg [10:0] J;	//counter to time dont_read_flag






//Index Counters

reg [12:0] i;  	//index for the FIFO memory as it is being read in

reg [7:0] P;	//bit index for initialize parameters

reg [12:0] N;  	//index for the FIFO memory being read out

reg [4:0] E;	//index for each full sweep cycle (holds 20)







//This register is the most significant in the whole program.  The program collects the data from 
//the rabbit and stores it in this register and then	reads the data from
//this register when sending it to the DDS.
//The register has 3680 bits because it is capable of storing 20 sweeps (20*184=3680).  However,
//not all of these bits will be used if storing less than 20 sweeps.
				
reg [0:5519] memory_reg;
					





/********
The following 1-bit registers are all flags which allow the program to run its different functions 
without interfering with itself.

"init_key_flag"				This flag is high when the initializing data is being sent to the DDS.
							"B" increments while this flag is high.
							"init_end_flag" == 0 is a condition of this flag being set.  
							"init_end_flag" goes high when "B>=1000" which clears "init_key_flag,"
							stopping "B" from incrementing and ending the initialize stage.

"init_end_flag"				This flag goes high when "B">=1000 and is low when B<1000. 
							This flag begin set coincides with "B" being reset to 0,
							so it is only high for one clock cycle, where it resets
							"init_key_flag"
					
"dont_read_flag"			This flag goes high when the "init_trigger" signal occurs, and it
							it increments the counting register "J" which counts up to 
							2000 clock ticks (2000*100ns=.2ms) at which time the flag is cleared.
							When this flag is high no data will be read into "memory_reg."
							During the initialize sequence, the "RABBIT_CLOK" clock signal
							comes from the RABBIT.  This is necessary so that "i" and "memory_reg" 
							can be reset within the proper "always" loop, after a multiple sweep 
							sequence finishes and the "reset_flag" is set.  Consequently, the 
							INITIALIZE commnand in the GUI will reset the FPGA after a sequence 
							of multiple sweeps, so it can load a new sequence of sweeps.
					
"sweep_trig_flag"			This flag is high when each 184-bit sweep data message is being sent 
							to the DDS. "T" increments while this flag is high.  This flag is 
							triggered by the "sweep_trig" trigger, however "sweep_end_flag" == 0 
							is also a condition of this flag being set. "sweep_end_flag" goes high 
							when "T>=20000000" which clears "sweep_trig_flag," stopping "T" from 
							incrementing and ending the sweep message stage.

"sweep_routine_end_flag"	This flag signifies that all of the data in "memory_reg" has been read.
							Since this happens before "IO_UPDATE" occurs there must be a wait period
							(counted by "G") before halting the incrementation of "T" so "IO_UDATE" 
							and "DR_CTL" are administered correctly in the last sweep of the routine.
							
"E_stop_flag" 				This flag occurs when triggered by the "sweep_routine_end_flag."  It causes
							"G" to increment and count the amount of time that T must increment for to 
							allow for "IO_UDATE" and "DR_CTL" but not get to the point that "T" reaches 
							20,000,000.  This insures that "E," the sweep number index, is not incremented 
							after the last sweep.  At the end of the "G" count, "end_routine_stop" goes high.
							
"end_routine_stop"			When "end_routine_stop" goes high, after the last sweep in a routine, it stops
							"T" from incrementing(l.), makes sure "sweep_trig_flag" stays low(l.), and it
							allows "reset_flag" to go high when the INITIALIZE command from the GUI is sent
							which resets the index "i" and "memory_reg"
				

*********/

output reg init_key_flag;			//flag occurs when initializing data is being sent to the DDS




output reg dont_read_flag;			//protects "memory_reg" from being written to when "i" and "memory_reg"
							//are being reset


output reg sweep_trig_flag; 		// This is high during the time that the sweep info is being sent out to the DDS
							
					


output reg sweep_routine_end_flag;  //this flag will occur when the whole memory_reg has been sent (when N>=i)
						//it resets the N and i indices to zero so the thing can be restarted
	
output reg E_stop_flag;


output reg end_routine_stop;


output reg reset_flag;


//(* ram_init_file = "Mif2.mif" *) reg [0] memory_reg [0:3679];



/***********
I have included this "initial" block to start all of the flags and counters at zero.
The initial bocks are "not synthesizeable" in Quartus II, which means you can't use them
to actually start a variable at a certain value when you load it on the FPGA; however, in
some cases when they aren't set, Quartus II will simplify the code by starting a variable 
at a non-zero value and possibly make your code not do what you intend it to do.
This error is usually accompanied by errors that say "output pins are stuck to ground
or VCC," "no input driving pin... ," "...variable assigned a value but not read,"
"registers simplified and merged into one register"

Initializing values at *zero* is useful to avoid Quartus rendering your code useless.
**********/
initial
	begin
	i<=0;  //index for the memory as it is being read in
	N<=0;  // index for the memory being read out
	M<=0; //tells when to send out the SDIO signal
	L<=0; //tells when to toggle the SCLK signal
	T<=0;
	B<=0;//time counter for initialize
	P<=0;//index for initialize
	J<=0;
	G<=0;
	E<=0;
	init_key_flag<=0;
	
	dont_read_flag<=0;
	sweep_trig_flag<=0;
	
	sweep_routine_end_flag<=0;
	E_stop_flag<=0;	
	end_routine_stop<=0;
	reset_flag<=0;
	end				
					


//this section of code deals with making the decisions about when to start doing what
//its main job is to make sure the FPGA knows when to send the signal to the DDS
// and when to reset everything
always@ (posedge ten_MHz_ext)
	begin
	
	IO_trigger_out<=IO_UPDATE/*!sweep_trig*/;
	
	CSB <=0;//must be tied to zero to have DDS recieve
	OSK <=0;
	DR_HOLD <=0;//must be tied to zero to let the DDS work smooth


	//this sets the sweep_trig_flag when the key_3 is depressed and sweep_trig_flag
	// has not been set in 1 sec to "debounce" the keys. sweep_end_flag is
	//set when sweep_trig_flag has been set for 1 sec
	if ((sweep_trig!= 0) && (end_routine_stop == 0))
		begin
		sweep_trig_flag <=1;
		end
	if (sweep_trig_flag != 0)
		begin
		//T increments while sweep_trig_flag is high. when T >=5,000,000 sweep_trig_flag will go back to zero
		T<=T+1'b1;
		end
		

	
	
	//this sets the init_key_flag  when the rabbit sends the initalize signal
	//don't know how init_end_flag should be used
	if /*(*/(init_trigger != 0) /*&& (init_end_flag == 0))*/
		begin
		init_key_flag <=1;
		end
	
	if (init_key_flag != 0)
		begin
		B<=B+1'b1;
		end
	/******
	After the INITIALIZE command is sent from the RABBIT the RABBIT send 16 clock cycles
	so that the lower always loop can clear out "i" and "memory_reg" the "dont_read_flag"
	allows those registers to be cleared without "false zeros" being read into them from the RABBIT's clock 
	signals
	******/	
	if (init_trigger !=0)
		begin
		dont_read_flag<=1;
		end
	if (dont_read_flag!=0)
		begin
		J<=J+1'b1;
		end
	if (J>=2000)//don't change this number unless you understand the way the timing diagram works;
		begin		//also, it very closely mathes up with the two bytes being sent from the RABBIT.
		J<=0;
		dont_read_flag<=0;
		end
	//this section will set the sweep_routine_end_flag which will indicate that the whole set of sweep has been run
	//this happens when N>=i this will reset everything important including setting N and i back to Zero
	if ((N>=i)&& (sweep_trig_flag !=0))
		begin
		sweep_routine_end_flag <=1;
		end
	if (N<i)
		begin
		sweep_routine_end_flag<=0;
		end
	
	if (end_routine_stop !=0)
		begin
		sweep_trig_flag<=0;
		end




	






//this section of code deals with sending out the signal to the DDS 

	//possible trouble shooting idea?
	//i_lsb<=i[0];
	
	/******
	This part of the code deals with the intialising parameters being sent to the DDS.  
	This specifies the settings we need on the DDS.
	The actual signal that must be sent to the DDS is specified above in the parameter,
	
	"INIT_REG = {8'h0b,32'h0,32'h0,8'h00,32'h00001000,8'h01,32'h00480020,
								8'h02,32'h353fc1c8,8'h03,32'h0}; " 
	
	The code below is sequenced by a counting register, B, which is used to time the signals being sent to
	the following pins:
		IO_RESET
		SDIO
		SCLK
		IO_UPDATE
	Note that there is no change in the DR_CTL signal because that pin only deals with actual sweeps.
	Also IO_RESET is only used during this cycle
	
	
	
	
	*****/
	
		
	
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
		if(M>=1)
			begin
			M<=0;
			SCLK<=1;
			P<= P+1'b1;
			end
		if(M<1)
			begin
			M<=M+1'b1;
			SCLK<=0;
			SDIO<=INIT_REG[P];
			end
		end
	if ((B>=622)&&(B<702))
		begin
		if(M>=1)
			begin
			M<=0;
			SCLK<=1;
			P<=P+1'b1;
			end
		if(M<1)
			begin
			M<=M+1'b1;
			SCLK<=0;
			SDIO<=UNCLEAR_REG[P];
			end
		end
	if ((((B<55) || (B>=519))&&((B<622)||(B>=702)))&&(sweep_trig_flag == 0))
		begin
		P<=0;
		SCLK<=0;
		SDIO<=0;
		end
	if (((B>= 568) && (B<573)) || ((B>=751) && (B<756)))
		begin
		IO_UPDATE <= 1;
		end
	if ((((B<568) || (B>=573))&&((B<751)||(B>=756)))&&(sweep_trig_flag == 0))
		begin
		IO_UPDATE <=0;
		end		
	if (B>=1000)
		begin
		init_key_flag<=0;
		B<=0;
		if (end_routine_stop!=0)
			begin
			reset_flag<=1;
			end
		end
		
		
	if (i<=1)
		begin
		reset_flag<=0;
		end
	


	


	
	
	
/*******
The code below is sequenced by a counting register, T, which is used to time the signals being sent to
	the following pins:
		DR_CTL
		SDIO
		SCLK
		IO_UPDATE
	

*********/

	
		
		
		
		
		
	
			
			
			
			
			
			
			
			
			
			
	//This is the section of code where the 184-bit sweep message is actually being sent.
	//It reults in the serial clock from the FPGA having twice the period of the 
	//external 10MHz clock this "always" loop runs off of.
	//N is not reset after one sweep, but gets larger with each sweep until
	//it runs through all "i" data bits.
	if ((T>=5) && (T<373))
		begin
		if(L>=1)
			begin
			L<=0;
			SCLK<=1;
			N<= N+1'b1;
			end
		if(L<1)
			begin
			L<=L+1'b1;
			SCLK<=0;
			SDIO<=memory_reg[N];
			end
		end
	
	
	
	
	//SCLK and SDIO are kept low when no message is being transmitted.
	
	if (((T<5) || (T>=373))&&(init_key_flag == 0))
		begin
		SCLK<=0;
		SDIO<=0;
		end
		
		
		
	//This is the section of code that toggles the IO_UPDATE after the sweep data has been sent to the 
	//buffers of the DDS. This loads the data into the active registers.  The direction of DR_CTL
	//at this point determines the polarity of the sweep.
	if ((T>= 422) && (T<423))
		begin
		IO_UPDATE <= 1;
		end
	if (((T<422) || (T>=423)) && (init_key_flag == 0))
		begin
		IO_UPDATE <=0;
		end	
				
		
			
		
	
							
	//Right now this goes up to 20000000 to "debounce"  the key on the DE2 I use to trigger the sweeping.
	//Once there is a trigger that comes from the network this can be a little greater than the number
	//"T" at which "E_stop_flag" would go high on the last sweep of a routine.
				
							
	if (T==500) 
		begin
		T<=0;
		E<=E+1'b1;
		sweep_trig_flag<=0;
		end

	//After a whole sweep routine is done and therefore "sweep_routine_end_flag" goes high because
	//the sent index "N" is greater than the reading index "i," "E_stop_flag" goes high to let "T" increment
	//enough to have "IO_UPDATE" and "DR_CTL" work for the last sweep, but not reach the point where 
	//"E," the sweep number index is incremented.
	if (sweep_routine_end_flag !=0)
		begin
		E_stop_flag<=1;
		N<=0;
		end
	if (E_stop_flag !=0)
		begin	
		G<=G+1'b1;
		end
	//After "G" reaches 63, "E_stop_flag" goes low and "end_routine_stop" stops "T" from incrementing
	//and makes "sweep_trig_flag" low until after the INITALIZE command comes from the RABBIT
	//again.	
	if (G>=63)
		begin
		E_stop_flag<=0;
		G<=0;
		end_routine_stop<=1;
		T<=0;
		end
		
		
		
		
		
		
	//"reset_flag" gets set to one when an INITIALIZE command comes from the RABBIT and "end_routine_stop"
	//is high.  This flag resets "E" and "end_routine_stop" here, and resets "i" and "memory_reg"
	//within the "always" loop below, based on the RABBIT's serial data clock
	if (reset_flag !=0)
		begin
		E<=0;
		end_routine_stop<=0;
		end
	
	
		
		
		
		
		
		

/*******
The following code is a large decision tree whose function is to set and clear the DR_CTL pin 
at the appropriate times.  
Particularly, for a High to Low sweep, it sets the DR_CTL pin high just slightly after the IO_UPDATE
pin is set high which results in the DDS signal to snap to the lower limit for 12ns immediately
after the IO_UPDATE toggling occurs.  Then the DDS signal snaps to the upper limit of the sweep
loaded by the IO_UPDATE toggling and perfoms the sweep indicated.

This window of time of the IO_UPDATE (422<T<423;100ns) is the beginning of each sweep sent to the DDS.
The decision process below determines whether the DDS does an up or down sweep by controlling the DR_CTL
pin.  The decision tree compares the section of the 184-bit sweep information which corresponds to
the DIGITAL RAMP STEP SIZE for the inclination and declination.  
The 184-bit code from the rabbit sets the STEP SIZE to *maximum* (FFFFFFFF) for the ramping direction
that is *not* being implemented.

The DIGITAL RAMP STEP SIZE REGISTER has the address 0x0c, written in verilog hdl using hexidecimal
as the byte, 8'h0c, or in binary as 8'b00001100.  
When this address is sent over the SDIO line to the DDS, the subsequent 64 bits are read into the
DIGITAL RAMP STEP SIZE REGISTER.  The first 32 bits correspond to the STEP SIZE for declination,
and the second 32-bits correspond to the STEP SIZE for inclination.

i.e. 
For a high to low sweep the first 32-bits after the DR STEP SIZE address (8'h0c) are the STEP SIZE
referring to the rate of the high to low sweep.  The second 32 bits in a high to low sweep will
always be the MAXIMUM possible STEP SIZE (32'hFFFFFFFF).

For a low to high sweep the first 32-bits after the DR STEP SIZE address (8'h0c) are always going to be
the *maximum* possible STEP SIZE (32'hFFFFFFFF), and the second 32 bits are the STEP SIZE referring to the
rate of the low to high sweep.

Therefore, the decision tree below compares the parts of the FIFO register (memory_reg) that contain
these STEP SIZE bits for each sweep stored in the FPGA and then uses the comparison to determine which 
DR_CTL bit pattern to use.

The first layer of decision determines that the DR_CTL manipulation happens at the right time (422=<T<430) 
or (430<T).

The second layer of decision determines which sweep the FPGA is sending to the DDS, designated by
the variable E.

The third layer of decision determines which way the sweep set to go and then performs the setting or 
clearing of the DR_CTL pin as needed.

For a high to low sweep the DR_CTL pin is set immediately as the IO_UPDATE occurs (T==422) and then is cleared
a few clock cycles later (T==430).

For a low to high sweep the DR_CTL pin is cleared immediately as the IO_UPDATE occurs (T==422)and a few clock 
cycles later is set to high (T==430).



The rule here is for every sweep "E" {E|0:19}

	*If "memory_reg[184*E + 8:  184*E + 39] > memory_reg[184*E + 40: 184*E + 71]" is true
	the sweep is from low to high. 

	*If "memory_reg[184*E + 8:  184*E + 39] < memory_reg[184*E + 40: 184*E + 71]" is true
	the sweep is from high to low.
	
	
(The above decision process could not be implemented specifically because verilog hdl does not allow
a variable, E, to go into indices of a register in that way when there is a colon/, [184*E + 8: 184*E + 39])
*********/
	if ((T>=422) && (T<430))
			begin
			
			
			if (E<=28)
	begin
	if (E<=27)
		begin
		if (E<=26)
			begin
			if (E<=25)
				begin
				if (E<=24)
					begin
					if (E<=23)
						begin
						if (E<=22)
							begin
							if (E<=21)
								begin
								if (E<=20)
									begin
									if (E<=19)
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
															begin//5
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
																				DR_CTL<=!memory_reg[8];																				
																				end
																			else
																				begin
																				DR_CTL<=!memory_reg[192];
																				end
																			end
																		else
																			begin
																			DR_CTL<=!memory_reg[376];
																			end	
																		end
																	else
																		begin
																		DR_CTL<=!memory_reg[560];
																		end	
																	end 
																else
																	begin
																	DR_CTL<=!memory_reg[744];
																	end		
																end						
															else
																begin
																DR_CTL<=!memory_reg[928];
																end	
															end
														else
															begin
															DR_CTL<=!memory_reg[1112];
															end	
														end
													else
														begin
														DR_CTL<=!memory_reg[1296];
														end	
													end
												else
													begin
													DR_CTL<=!memory_reg[1480];
													end	
												end
											else
												begin
												DR_CTL<=!memory_reg[1664];
												end	
											end
									else
										begin
										DR_CTL<=!memory_reg[1848];
										end	
									end
								else
									begin
									DR_CTL<=!memory_reg[2032];
									end		
								end
							else
								begin
								DR_CTL<=!memory_reg[2216];
								end		
							end
						else
							begin
							DR_CTL<=!memory_reg[2400];
							end		
						end 
					else
						begin
						DR_CTL<=!memory_reg[2584];
						end		
					end							
				else
					begin
					DR_CTL<=!memory_reg[2768];
					end	
				end
			else
				begin
				DR_CTL<=!memory_reg[2952];
				end	
			end
		else
			begin
			DR_CTL<=!memory_reg[3136];
			end	
		end
	else
		begin
		DR_CTL<=!memory_reg[3320];
		end	
	end
else
	begin
	DR_CTL<=!memory_reg[3504];
	end
end	
else
	begin
	DR_CTL<=!memory_reg[3688];
	end
		end
	else
	begin
	DR_CTL<=!memory_reg[3872];
	end
		end	
		else
	begin
	DR_CTL<=!memory_reg[4056];
	end
		end
		else
	begin
	DR_CTL<=!memory_reg[4240];
	end
		end
		else
	begin
	DR_CTL<=!memory_reg[4424];
	end
		end
		else
	begin
	DR_CTL<=!memory_reg[4608];
	end
		end
		else
	begin
	DR_CTL<=!memory_reg[4792];
	end
		end
		else
	begin
	DR_CTL<=!memory_reg[4976];
	end
		end
		else
	begin
	DR_CTL<=!memory_reg[5160];
	end
		end
		else
	begin
	DR_CTL<=!memory_reg[5344];
	end
		end
	

	if (T==430)
		begin
		DR_CTL<=!DR_CTL;
		end
		
		
	end
	
	
	
		

		
/********
This section of code reads all the messages from the RABBIT.
As is apparent, this "always" loop runs off of the positive edge of the RABBIT SERIAL DATA CLOCK,
so these actions only execute when the RABBIT is send some message.

The "dont_read_flag" protects the "memory_reg" from having data read into it during the
time in the initialize cycle where the RABBIT must send out a SERIAL DATA CLOCK signal.

When the initialize command is sent from the GUI to the RABBIT, the RABBIT sends out a number of clock
cycles, which, when the "reset_flag" is high, resets the "i" index and "memory_reg."  It is important
that the "dont_read_flag" is high when this occurs so that the "memory_reg" doesn't get zeros read
into it during this time.

This method of reseting the "i" index and "memory_reg" is necessary because Verilog HDL demands that
a variable must be written to in no more than one "always" loop, and this "always" loop only runs off
of the RABBIT's SERIAL DATA CLOCK.
*********/
always@ (posedge RABBIT_CLOK)
	begin
	
	if (reset_flag !=0)
		begin
		i<=0;
		memory_reg<= 5520'b0;
		end
	
			
	if (dont_read_flag == 0)			
		begin
		memory_reg[i] <= RABBIT_DATA;
		i<=i+1'b1;
		end
	end
		

			
endmodule			
	

