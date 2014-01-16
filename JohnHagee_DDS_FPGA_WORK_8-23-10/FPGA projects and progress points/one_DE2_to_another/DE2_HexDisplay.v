/*//module for displaying the number on the HEX display


module DE2_HexDisplay( display_data, HexDisplay_output);

	input [17:0] display_data; // input wire bringing in data that has been read in a parallel way
	output reg [41:0] HexDisplay_output;  // output wires to turn on apprioptriate bits in HEX display
	
	
	//
	reg [6:0] display0;
	reg [6:0] display1;
	reg [6:0] display2;
	reg [6:0] display3;
	reg [6:0] display4;
	reg [6:0] display5;
		
	reg [17:0] tmp_division;
	reg [3:0] tmp_remainder0, tmp_remainder1, tmp_remainder2, tmp_remainder3, tmp_remainder4, tmp_remainder5;
	
// initialize the display (i.e. turn all the LEDs off: High = off, Low = on)
	initial
		begin
		HexDisplay_output = 42'b111111111111111111111111111111111111111111;
		end
		
	always
		begin
		// convert input_data in units of 10's of Hz to kHz (i.e. divide by 100)
		tmp_division = display_data; 

		// 1s display
		tmp_remainder0 = tmp_division % 4'b1010;
		display0 = output_singledisplay_7bit(tmp_remainder0);
		
		// 10s display
		tmp_division = tmp_division / 4'b1010;
		tmp_remainder1 = tmp_division % 4'b1010;
		display1 = output_singledisplay_7bit(tmp_remainder1);

		// 100s display
		tmp_division = tmp_division / 4'b1010;
		tmp_remainder2 = tmp_division % 4'b1010;
		display2 = output_singledisplay_7bit(tmp_remainder2);
		
		// 1000s display
		tmp_division = tmp_division / 4'b1010;
		tmp_remainder3 = tmp_division % 4'b1010;
		display3 = output_singledisplay_7bit(tmp_remainder3);

		// 10000s display
		tmp_division = tmp_division / 4'b1010;
		tmp_remainder4 = tmp_division % 4'b1010;
		display4 = output_singledisplay_7bit(tmp_remainder4);
		
		// 100000s display
		tmp_division = tmp_division / 4'b1010;
		tmp_remainder5 = tmp_division % 4'b1010;
		display5 = output_singledisplay_7bit(tmp_remainder5);


		// eliminate zeros on the left
		
		
		if (tmp_remainder5 == 4'b0000)
			begin
			display5 = 7'b1111111;
			if (tmp_remainder4 == 4'b0000)
				begin
				display4 = 7'b1111111;
				if (tmp_remainder3 == 4'b0000)
					begin
					display3 = 7'b1111111;
					if (tmp_remainder2 == 4'b0000)
						begin
						display2 = 7'b1111111;
						if (tmp_remainder1 == 4'b000)
							begin
							display1 = 7'b1111111;
							end
						end
					end
				end
			end
		// Generate Display output 
		HexDisplay_output = {display5,display4,display3,display2,display1,display0};
		
		end

	function [6:0] output_singledisplay_7bit;
		input [3:0] input_number_4bit;
		begin
		output_singledisplay_7bit = 7'b1111111;

		if (input_number_4bit == 4'b0000)
			output_singledisplay_7bit = 7'b1000000;

		if (input_number_4bit == 4'b0001)
			output_singledisplay_7bit = 7'b1111001;
	
		if (input_number_4bit == 4'b0010)
			output_singledisplay_7bit = 7'b0100100;
					  
		if (input_number_4bit == 4'b0011)
			output_singledisplay_7bit = 7'b0110000;
					  
		if (input_number_4bit == 4'b0100)
			output_singledisplay_7bit = 7'b0011001;
		
		if (input_number_4bit == 4'b0101)
			output_singledisplay_7bit = 7'b0010010;
	
		if (input_number_4bit == 4'b0110)
			output_singledisplay_7bit = 7'b0000010;
	
		if (input_number_4bit == 4'b0111)
			output_singledisplay_7bit= 7'b1111000;
	
		if (input_number_4bit == 4'b1000)
			output_singledisplay_7bit = 7'b0000000;
	
		if (input_number_4bit == 4'b1001)
			output_singledisplay_7bit = 7'b0011000;

		end

	endfunction

endmodule


//module SingleHexDisplay(input_number_4BIT, output_singledisplay_7bit);
//
//	input [3:0] input_number_4BIT;
//	output [6:0] output_singledisplay_7bit;
//
//	assign output_singledisplay_7bit = display_result(input_number_4BIT);
//
//	function [6:0] display_result;
//	
//		input [3:0] input_number_4Bit;
//
//		begin
//		output_singledisplay_7bit = 7'b1111111;
//
//		if (input_number_4bit == 4'b0000)
//			output_singledisplay_7bit = 7'b1000000;
//
//		if (input_number_4bit == 4'b0001)
//			output_singledisplay_7bit = 7'b1111001;
//	
//		if (input_number_4bit == 4'b0010)
//			output_singledisplay_7bit = 7'b0100100;
//					  
//		if (input_number_4bit == 4'b0011)
//			output_singledisplay_7bit = 7'b0110000;
//					  
//		if (input_number_4bit == 4'b0100)
//			output_singledisplay_7bit = 7'b0011001;
//		
//		if (input_number_4bit == 4'b0101)
//			output_singledisplay_7bit = 7'b0010010;
//	
//		if (input_number_4bit == 4'b0110)
//			output_singledisplay_7bit = 7'b0000010;
//	
//		if (input_number_4bit == 4'b0111)
//			output_singledisplay_7bit= 7'b1111000;
//	
//		if (input_number_4bit == 4'b1000)
//			output_singledisplay_7bit = 7'b0000000;
//	
//		if (input_number_4bit == 4'b1001)
//			output_singledisplay_7bit = 7'b0011000;
//
//		end
//
//	endfunction
//
//endmodule
*/

