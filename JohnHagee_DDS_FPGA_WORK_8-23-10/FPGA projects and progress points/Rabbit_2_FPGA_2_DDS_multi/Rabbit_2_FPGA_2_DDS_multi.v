module Rabbit_2_FPGA_2_DDS_multi( ten_MHz_ext, SCLK_PE_3, SDIO_PE_5, key_0_init,key_2_reset, key_3_sweep, key_1_trigger, 
SDIO, SCLK,DR_CTL, IO_UPDATE, IO_RESET,CSB, trigger, DR_HOLD, OSK,p_number,tell_sweep_flag, total);


input ten_MHz_ext;
input SCLK_PE_3; //gpio I 28
input SDIO_PE_5;//gpio I 6
input key_0_init;
input key_3_sweep;
input key_1_trigger; 
input key_2_reset;
output SDIO; 
output SCLK; 
output DR_CTL; 
output IO_UPDATE; 
output IO_RESET; 
output CSB; 
output trigger; 
output DR_HOLD; 
output OSK;
output [25:0] p_number; 
output tell_sweep_flag;
output [4:0] total;
wire [0:183] full_184_bit_choice;
wire [0:183] full_184_bit_0;
wire [0:183] full_184_bit_1;
wire [0:183] full_184_bit_2;
wire [0:183] full_184_bit_3;
wire [0:183] full_184_bit_4;
wire [0:183] full_184_bit_5;
wire [0:183] full_184_bit_6;
wire [0:183] full_184_bit_7;
wire [0:183] full_184_bit_8;
wire [0:183] full_184_bit_9;
wire [0:183] full_184_bit_10;
wire [0:183] full_184_bit_11;/*
wire [0:183] full_184_bit_12;
wire [0:183] full_184_bit_13;
wire [0:183] full_184_bit_14;
wire [0:183] full_184_bit_15;
wire [0:183] full_184_bit_16;
wire [0:183] full_184_bit_17;
wire [0:183] full_184_bit_18;
wire [0:183] full_184_bit_19;*/

wire [4:0] sweep_total;
wire [4:0] sweep_count;
wire sweep_count_over_flag;
wire init_key_flag;
assign total = sweep_count;
out_2_DDS outing_DDS(
ten_MHz_ext,
key_0_init,
key_2_reset,
key_3_sweep,
key_1_trigger, 
full_184_bit_0,
full_184_bit_1,
full_184_bit_2,
full_184_bit_3,
full_184_bit_4,
full_184_bit_5,
full_184_bit_6,
full_184_bit_7,
full_184_bit_8,
full_184_bit_9,
full_184_bit_10,
full_184_bit_11,/*
full_184_bit_12,
full_184_bit_13,
full_184_bit_14,
full_184_bit_15,
full_184_bit_16,
full_184_bit_17,
full_184_bit_18,
full_184_bit_19,*/
sweep_total,
SDIO,
SCLK,
DR_CTL, 
IO_UPDATE,
IO_RESET,
CSB,
trigger,
DR_HOLD,
OSK,
sweep_count_over_flag,init_key_flag,p_number,tell_sweep_flag
);


reader_multi reading_multi(init_key_flag,ten_MHz_ext, sweep_count_over_flag,
SCLK_PE_3, 
SDIO_PE_5, sweep_total,	full_184_bit_choice,/*
full_184_bit_0,
full_184_bit_1,
full_184_bit_2,
full_184_bit_3,
full_184_bit_4,
full_184_bit_5,
full_184_bit_6,
full_184_bit_7,
full_184_bit_8,
full_184_bit_9,
full_184_bit_10,
full_184_bit_11,
full_184_bit_12,
full_184_bit_13,
full_184_bit_14,
full_184_bit_15,
full_184_bit_16,
full_184_bit_17,
full_184_bit_18,
full_184_bit_19,*/

sweep_count
);


sorter sorting(

sweep_count,
full_184_bit_choice,
full_184_bit_0,
full_184_bit_1,
full_184_bit_2,
full_184_bit_3,
full_184_bit_4,
full_184_bit_5,
full_184_bit_6,
full_184_bit_7,
full_184_bit_8,
full_184_bit_9,
full_184_bit_10,
full_184_bit_11/*,
full_184_bit_12,
full_184_bit_13,
full_184_bit_14,
full_184_bit_15,
full_184_bit_16,
full_184_bit_17,
full_184_bit_18,
full_184_bit_19*/);

endmodule


