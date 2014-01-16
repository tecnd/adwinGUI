module sorter(
sweep_count_2,
full_184_bit_choice_2,
full_184_bit_2_0,
full_184_bit_2_1,
full_184_bit_2_2,
full_184_bit_2_3,
full_184_bit_2_4,
full_184_bit_2_5,
full_184_bit_2_6,
full_184_bit_2_7,
full_184_bit_2_8,
full_184_bit_2_9,
full_184_bit_2_10,
full_184_bit_2_11/*,
full_184_bit_2_12,
full_184_bit_2_13,
full_184_bit_2_14,
full_184_bit_2_15,
full_184_bit_2_16,
full_184_bit_2_17,
full_184_bit_2_18,
full_184_bit_2_19*/);


input [4:0] sweep_count_2;
input [0:183] full_184_bit_choice_2;

output reg [0:183] full_184_bit_2_0;
output reg [0:183] full_184_bit_2_1;
output reg [0:183] full_184_bit_2_2;
output reg [0:183] full_184_bit_2_3;
output reg [0:183] full_184_bit_2_4;
output reg [0:183] full_184_bit_2_5;
output reg [0:183] full_184_bit_2_6;
output reg [0:183] full_184_bit_2_7;
output reg [0:183] full_184_bit_2_8;
output reg [0:183] full_184_bit_2_9;
output reg [0:183] full_184_bit_2_10;
output reg [0:183] full_184_bit_2_11;/*
output reg [0:183] full_184_bit_2_12;
output reg [0:183] full_184_bit_2_13;
output reg [0:183] full_184_bit_2_14;
output reg [0:183] full_184_bit_2_15;
output reg [0:183] full_184_bit_2_16;
output reg [0:183] full_184_bit_2_17;
output reg [0:183] full_184_bit_2_18;
output reg [0:183] full_184_bit_2_19;*/



	/*reg [0:183] full_184_bit_2_0;
	reg [0:183] full_184_bit_2_1;
	reg [0:183] full_184_bit_2_2;
	reg [0:183] full_184_bit_2_3;
	reg [0:183] full_184_bit_2_4;
	reg [0:183] full_184_bit_2_5;
	reg [0:183] full_184_bit_2_6;
	reg [0:183] full_184_bit_2_7;
	reg [0:183] full_184_bit_2_8;
	reg [0:183] full_184_bit_2_9;
	reg [0:183] full_184_bit_2_10;
	reg [0:183] full_184_bit_2_11;
	reg [0:183] full_184_bit_2_12;
	reg [0:183] full_184_bit_2_13;
	reg [0:183] full_184_bit_2_14;
	reg [0:183] full_184_bit_2_15;
	reg [0:183] full_184_bit_2_16;
	reg [0:183] full_184_bit_2_17;
	reg [0:183] full_184_bit_2_18;
	reg [0:183] full_184_bit_2_19;*/

always 
	begin
	case (sweep_count_2)
		5'b00000: full_184_bit_2_0 = full_184_bit_choice_2; 
		5'b00001: full_184_bit_2_1 = full_184_bit_choice_2;
		5'b00010: full_184_bit_2_2 = full_184_bit_choice_2;
		5'b00011: full_184_bit_2_3 = full_184_bit_choice_2;
		5'b00100: full_184_bit_2_4 = full_184_bit_choice_2;
		5'b00101: full_184_bit_2_5 = full_184_bit_choice_2;
		5'b00110: full_184_bit_2_6 = full_184_bit_choice_2;
		5'b00111: full_184_bit_2_7 = full_184_bit_choice_2;
		5'b01000: full_184_bit_2_8 = full_184_bit_choice_2;
		5'b01001: full_184_bit_2_9 = full_184_bit_choice_2;
		5'b01010: full_184_bit_2_10 = full_184_bit_choice_2;
		5'b01011: full_184_bit_2_11 = full_184_bit_choice_2;/*
		5'b01100: full_184_bit_2_12 = full_184_bit_choice_2;
		5'b01101: full_184_bit_2_13 = full_184_bit_choice_2;
		5'b01110: full_184_bit_2_14 = full_184_bit_choice_2;
		5'b01111: full_184_bit_2_15 = full_184_bit_choice_2;
		5'b10000: full_184_bit_2_16 = full_184_bit_choice_2;
		5'b10001: full_184_bit_2_17 = full_184_bit_choice_2;
		5'b10010: full_184_bit_2_18 = full_184_bit_choice_2;
		5'b10011: full_184_bit_2_19 = full_184_bit_choice_2;*/
		
	endcase
/*	
	alt_reader_func= 
	{full_184_bit_2_0,
	full_184_bit_2_1,
	full_184_bit_2_2,
	full_184_bit_2_3,
	full_184_bit_2_4,
	full_184_bit_2_5,
	full_184_bit_2_6,
	full_184_bit_2_7,
	full_184_bit_2_8,
	full_184_bit_2_9,
	full_184_bit_2_10,
	full_184_bit_2_11,
	full_184_bit_2_12,
	full_184_bit_2_13,
	full_184_bit_2_14,
	full_184_bit_2_15,
	full_184_bit_2_16,
	full_184_bit_2_17,
	full_184_bit_2_18,
	full_184_bit_2_19};
	end*/





	/*
	{full_184_bit_2_0,
	full_184_bit_2_1,
	full_184_bit_2_2,
	full_184_bit_2_3,
	full_184_bit_2_4,
	full_184_bit_2_5,
	full_184_bit_2_6,
	full_184_bit_2_7,
	full_184_bit_2_8,
	full_184_bit_2_9,
	full_184_bit_2_10,
	full_184_bit_2_11,
	full_184_bit_2_12,
	full_184_bit_2_13,
	full_184_bit_2_14,
	full_184_bit_2_15,
	full_184_bit_2_16,
	full_184_bit_2_17,
	full_184_bit_2_18,
	full_184_bit_2_19}  =alt_reader_func(sweep_count,full_184_bit_choice_2);*/
	end
endmodule
