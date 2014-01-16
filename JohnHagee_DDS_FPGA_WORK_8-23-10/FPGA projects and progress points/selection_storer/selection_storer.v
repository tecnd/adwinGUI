//This project will recieve a 12-bit number from the  switches 0-11 which will be stored in one of 8 spots
//according to a 3-bit number from switches 17-15
// there are 3 more switches and one can be used as a safety -- this switch will not allow the recieving 
//module to overwrite the number specified


//tasks include:
//reading number from switches
//sending number to be stored
//displaying number according to selection register
//storing number according to selection register
//so we need two triggers



//inputs needed:
/* clock going into the reading module, 
50 MHz internal clock,  
trigger for sending/writing,
switch inputs can be specified as input cables 
		(12 bit number, selection number, saftey switch, etc.)
		
data sending line
flag for data transfer
trigger for displaying
two key inputs
*/

//outputs needed
/*
clock coming from sending module
flag from sending module
hex displays
trigger output to send number
trigger output to display number
data in serial*/


//modules needed
/*
data_switch_send: (like data_program) 
desc: reads switches. sends switches serially when it is triggered. sends flag. ?sends synch clock? 
only diff--running tenMHz output clock thru it
I: ten_MHz_clock(wire) , sending_trigger_input, switch_number_input,
O: ten_MHz_synch_output?, serial_output, flag_output


clock_maker: (same as previous) 
desc: takes in 50MHz, puts out 10MHz
I: fifty_MHz_int_clock
O: ten_MHz_synch_output (wire?)


data_read_write:  
desc: like read_data, but can write multiple slots depending on queue register
also will send display number to hex display program when a trigger comes according to same register,
also will not write when the saftey switch is on
I: ten_MHz_synch_input, flag_input, serial_input, queue, saftey_switch
O: Hex_display_number(wire?)


Hex_displayer:
desc: will display any number that data_read_write sends in cable Hex_display_number
improved so all zeros on left
I: clock? , Hex_display_number
O: Hex_displays



trigger_program
desc:
inputs two keys for two triggers.  one to data_switch_send and one to 
data_read_write
I: fifty_MHz_int_clock, key_0, key_2
O: sending_trigger_output, displaying_trigger_output






*/



module selection_storer(fifty_MHz_int_clock, ten_MHz_synch_input, sending_trigger_input, switch_number_input, 
queue, key_0, key_2, saftey_switch, serial_input, displaying_trigger_input, data_ctrl_input,
ten_MHz_synch_output, data_ctrl_output, Hex_displays, sending_trigger_output, displaying_trigger_output, 
serial_output);

//inputs:

	//
input fifty_MHz_int_clock;

input ten_MHz_synch_input;

input sending_trigger_input;

input [11:0] switch_number_input;

input [2:0] queue;

input saftey_switch;

input serial_input;

input displaying_trigger_input;

input data_ctrl_input;

input key_0, key_2;




//outputs:

	//
output ten_MHz_synch_output;

output data_ctrl_output;

output [55:0] Hex_displays;

output sending_trigger_output;

output displaying_trigger_output;

output serial_output;

//wires:

	//
/**************** clock wire will probably not work!!!!
**************************************/
	
wire ten_MHz_clock;
wire [11:0] Hex_display_number;


data_switch_send data_switch_sending(fifty_MHz_int_clock, ten_MHz_clock, sending_trigger_input, switch_number_input,
ten_MHz_synch_output, serial_output, data_ctrl_output);


clock_maker clock_making(fifty_MHz_int_clock,
ten_MHz_clock);


data_read_write data_reading_writing(ten_MHz_synch_input, data_ctrl_input, serial_input, queue, saftey_switch, 
displaying_trigger_input, Hex_display_number);


Hex_displayer Hex_displaying( Hex_display_number, Hex_displays);

trigger_program trigger_programming(fifty_MHz_int_clock, key_0, key_2, sending_trigger_output,
 displaying_trigger_output);

endmodule

















