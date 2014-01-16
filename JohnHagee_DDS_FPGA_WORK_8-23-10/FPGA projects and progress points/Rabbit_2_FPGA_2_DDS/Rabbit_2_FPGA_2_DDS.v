module Rabbit_2_FPGA_2_DDS(ten_MHz_ext, SCLK_PE_3, SDIO_PE_5, key_0_init, key_3_sweep, key_1_trigger, 
SDIO, SCLK,DR_CTL, IO_UPDATE, IO_RESET,CSB, trigger, DR_HOLD, OSK);

input ten_MHz_ext;
input SCLK_PE_3; //gpio I 28
input SDIO_PE_5;//gpio I 6
input key_0_init;
input key_3_sweep;
input key_1_trigger; 
output SDIO; 
output SCLK; 
output DR_CTL; 
output IO_UPDATE; 
output IO_RESET; 
output CSB; 
output trigger; 
output  DR_HOLD; 
output OSK;

wire [0:183] full_184_bit;
wire init_key_flag;


out_2_DDS outing_2_DDS(ten_MHz_ext, key_0_init, key_3_sweep, key_1_trigger, full_184_bit,SDIO, SCLK,DR_CTL, 
IO_UPDATE,IO_RESET,CSB, trigger, DR_HOLD, OSK,init_key_flag);

reader reading(init_key_flag, SCLK_PE_3, SDIO_PE_5, full_184_bit);
endmodule


