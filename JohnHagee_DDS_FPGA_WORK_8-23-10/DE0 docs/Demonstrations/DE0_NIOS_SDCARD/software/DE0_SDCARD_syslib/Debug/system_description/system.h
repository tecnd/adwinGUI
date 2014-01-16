/* system.h
 *
 * Machine generated for a CPU named "cpu" as defined in:
 * d:\NIOS_II\DE0\CD_EXAMPLE\DE0_NIOS_SDCARD\software\DE0_SDCARD_syslib\..\..\DE0_SOPC.ptf
 *
 * Generated: 2009-05-19 20:46:55.125
 *
 */

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

/*

DO NOT MODIFY THIS FILE

   Changing this file will have subtle consequences
   which will almost certainly lead to a nonfunctioning
   system. If you do modify this file, be aware that your
   changes will be overwritten and lost when this file
   is generated again.

DO NOT MODIFY THIS FILE

*/

/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2003 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
*                                                                             *
******************************************************************************/

/*
 * system configuration
 *
 */

#define ALT_SYSTEM_NAME "DE0_SOPC"
#define ALT_CPU_NAME "cpu"
#define ALT_CPU_ARCHITECTURE "altera_nios2"
#define ALT_DEVICE_FAMILY "CYCLONEIII"
#define ALT_STDIN "/dev/jtag_uart"
#define ALT_STDIN_TYPE "altera_avalon_jtag_uart"
#define ALT_STDIN_BASE 0x02811020
#define ALT_STDIN_DEV jtag_uart
#define ALT_STDIN_PRESENT
#define ALT_STDOUT "/dev/jtag_uart"
#define ALT_STDOUT_TYPE "altera_avalon_jtag_uart"
#define ALT_STDOUT_BASE 0x02811020
#define ALT_STDOUT_DEV jtag_uart
#define ALT_STDOUT_PRESENT
#define ALT_STDERR "/dev/jtag_uart"
#define ALT_STDERR_TYPE "altera_avalon_jtag_uart"
#define ALT_STDERR_BASE 0x02811020
#define ALT_STDERR_DEV jtag_uart
#define ALT_STDERR_PRESENT
#define ALT_CPU_FREQ 50000000
#define ALT_IRQ_BASE NULL

/*
 * processor configuration
 *
 */

#define NIOS2_CPU_IMPLEMENTATION "fast"
#define NIOS2_BIG_ENDIAN 0

#define NIOS2_ICACHE_SIZE 2048
#define NIOS2_DCACHE_SIZE 2048
#define NIOS2_ICACHE_LINE_SIZE 32
#define NIOS2_ICACHE_LINE_SIZE_LOG2 5
#define NIOS2_DCACHE_LINE_SIZE 32
#define NIOS2_DCACHE_LINE_SIZE_LOG2 5
#define NIOS2_FLUSHDA_SUPPORTED

#define NIOS2_EXCEPTION_ADDR 0x02808020
#define NIOS2_RESET_ADDR 0x02808000
#define NIOS2_BREAK_ADDR 0x02810820

#define NIOS2_HAS_DEBUG_STUB

#define NIOS2_CPU_ID_SIZE 1
#define NIOS2_CPU_ID_VALUE 0

/*
 * A define for each class of peripheral
 *
 */

#define __ALTERA_AVALON_JTAG_UART
#define __ALTERA_AVALON_UART
#define __ALTERA_AVALON_ONCHIP_MEMORY2
#define __ALTERA_AVALON_SYSID
#define __ALTERA_AVALON_TIMER
#define __ALTERA_AVALON_LCD_16207
#define __ALTERA_AVALON_PIO
#define __ALTERA_AVALON_CFI_FLASH
#define __ALTERA_AVALON_TRI_STATE_BRIDGE
#define __ALTERA_AVALON_PLL
#define __ALTERA_AVALON_NEW_SDRAM_CONTROLLER
#define __ALTERA_AVALON_CLOCK_CROSSING

/*
 * jtag_uart configuration
 *
 */

#define JTAG_UART_NAME "/dev/jtag_uart"
#define JTAG_UART_TYPE "altera_avalon_jtag_uart"
#define JTAG_UART_BASE 0x02811020
#define JTAG_UART_SPAN 8
#define JTAG_UART_IRQ 0
#define JTAG_UART_WRITE_DEPTH 64
#define JTAG_UART_READ_DEPTH 64
#define JTAG_UART_WRITE_THRESHOLD 8
#define JTAG_UART_READ_THRESHOLD 8
#define JTAG_UART_READ_CHAR_STREAM ""
#define JTAG_UART_SHOWASCII 1
#define JTAG_UART_READ_LE 0
#define JTAG_UART_WRITE_LE 0
#define JTAG_UART_ALTERA_SHOW_UNRELEASED_JTAG_UART_FEATURES 0
#define ALT_MODULE_CLASS_jtag_uart altera_avalon_jtag_uart

/*
 * uart configuration
 *
 */

#define UART_NAME "/dev/uart"
#define UART_TYPE "altera_avalon_uart"
#define UART_BASE 0x00000020
#define UART_SPAN 32
#define UART_IRQ 4
#define UART_BAUD 115200
#define UART_DATA_BITS 8
#define UART_FIXED_BAUD 1
#define UART_PARITY 'N'
#define UART_STOP_BITS 1
#define UART_SYNC_REG_DEPTH 2
#define UART_USE_CTS_RTS 1
#define UART_USE_EOP_REGISTER 0
#define UART_SIM_TRUE_BAUD 0
#define UART_SIM_CHAR_STREAM ""
#define UART_FREQ 10000000
#define ALT_MODULE_CLASS_uart altera_avalon_uart

/*
 * onchip_mem configuration
 *
 */

#define ONCHIP_MEM_NAME "/dev/onchip_mem"
#define ONCHIP_MEM_TYPE "altera_avalon_onchip_memory2"
#define ONCHIP_MEM_BASE 0x02808000
#define ONCHIP_MEM_SPAN 30720
#define ONCHIP_MEM_ALLOW_MRAM_SIM_CONTENTS_ONLY_FILE 0
#define ONCHIP_MEM_RAM_BLOCK_TYPE "AUTO"
#define ONCHIP_MEM_INIT_CONTENTS_FILE "onchip_mem"
#define ONCHIP_MEM_NON_DEFAULT_INIT_FILE_ENABLED 0
#define ONCHIP_MEM_GUI_RAM_BLOCK_TYPE "Automatic"
#define ONCHIP_MEM_WRITEABLE 1
#define ONCHIP_MEM_DUAL_PORT 0
#define ONCHIP_MEM_SIZE_VALUE 30720
#define ONCHIP_MEM_SIZE_MULTIPLE 1
#define ONCHIP_MEM_USE_SHALLOW_MEM_BLOCKS 0
#define ONCHIP_MEM_INIT_MEM_CONTENT 1
#define ONCHIP_MEM_ALLOW_IN_SYSTEM_MEMORY_CONTENT_EDITOR 0
#define ONCHIP_MEM_INSTANCE_ID "NONE"
#define ONCHIP_MEM_READ_DURING_WRITE_MODE "DONT_CARE"
#define ONCHIP_MEM_IGNORE_AUTO_BLOCK_TYPE_ASSIGNMENT 1
#define ONCHIP_MEM_CONTENTS_INFO ""
#define ALT_MODULE_CLASS_onchip_mem altera_avalon_onchip_memory2

/*
 * sysid configuration
 *
 */

#define SYSID_NAME "/dev/sysid"
#define SYSID_TYPE "altera_avalon_sysid"
#define SYSID_BASE 0x000000e0
#define SYSID_SPAN 8
#define SYSID_ID 40899754u
#define SYSID_TIMESTAMP 1242736836u
#define SYSID_REGENERATE_VALUES 0
#define ALT_MODULE_CLASS_sysid altera_avalon_sysid

/*
 * timer configuration
 *
 */

#define TIMER_NAME "/dev/timer"
#define TIMER_TYPE "altera_avalon_timer"
#define TIMER_BASE 0x00000000
#define TIMER_SPAN 32
#define TIMER_IRQ 3
#define TIMER_ALWAYS_RUN 0
#define TIMER_FIXED_PERIOD 0
#define TIMER_SNAPSHOT 1
#define TIMER_PERIOD 1
#define TIMER_PERIOD_UNITS "ms"
#define TIMER_RESET_OUTPUT 0
#define TIMER_TIMEOUT_PULSE_OUTPUT 0
#define TIMER_LOAD_VALUE 9999
#define TIMER_COUNTER_SIZE 32
#define TIMER_MULT 0.0010
#define TIMER_TICKS_PER_SEC 1000
#define TIMER_FREQ 10000000
#define ALT_MODULE_CLASS_timer altera_avalon_timer

/*
 * lcd configuration
 *
 */

#define LCD_NAME "/dev/lcd"
#define LCD_TYPE "altera_avalon_lcd_16207"
#define LCD_BASE 0x00000090
#define LCD_SPAN 16
#define ALT_MODULE_CLASS_lcd altera_avalon_lcd_16207

/*
 * sd_clk configuration
 *
 */

#define SD_CLK_NAME "/dev/sd_clk"
#define SD_CLK_TYPE "altera_avalon_pio"
#define SD_CLK_BASE 0x000000a0
#define SD_CLK_SPAN 16
#define SD_CLK_DO_TEST_BENCH_WIRING 0
#define SD_CLK_DRIVEN_SIM_VALUE 0
#define SD_CLK_HAS_TRI 0
#define SD_CLK_HAS_OUT 1
#define SD_CLK_HAS_IN 0
#define SD_CLK_CAPTURE 0
#define SD_CLK_DATA_WIDTH 1
#define SD_CLK_RESET_VALUE 0
#define SD_CLK_EDGE_TYPE "NONE"
#define SD_CLK_IRQ_TYPE "NONE"
#define SD_CLK_BIT_CLEARING_EDGE_REGISTER 0
#define SD_CLK_BIT_MODIFYING_OUTPUT_REGISTER 0
#define SD_CLK_FREQ 10000000
#define ALT_MODULE_CLASS_sd_clk altera_avalon_pio

/*
 * sd_dat configuration
 *
 */

#define SD_DAT_NAME "/dev/sd_dat"
#define SD_DAT_TYPE "altera_avalon_pio"
#define SD_DAT_BASE 0x000000b0
#define SD_DAT_SPAN 16
#define SD_DAT_DO_TEST_BENCH_WIRING 0
#define SD_DAT_DRIVEN_SIM_VALUE 0
#define SD_DAT_HAS_TRI 1
#define SD_DAT_HAS_OUT 0
#define SD_DAT_HAS_IN 0
#define SD_DAT_CAPTURE 0
#define SD_DAT_DATA_WIDTH 1
#define SD_DAT_RESET_VALUE 0
#define SD_DAT_EDGE_TYPE "NONE"
#define SD_DAT_IRQ_TYPE "NONE"
#define SD_DAT_BIT_CLEARING_EDGE_REGISTER 0
#define SD_DAT_BIT_MODIFYING_OUTPUT_REGISTER 0
#define SD_DAT_FREQ 10000000
#define ALT_MODULE_CLASS_sd_dat altera_avalon_pio

/*
 * sd_cmd configuration
 *
 */

#define SD_CMD_NAME "/dev/sd_cmd"
#define SD_CMD_TYPE "altera_avalon_pio"
#define SD_CMD_BASE 0x00000080
#define SD_CMD_SPAN 16
#define SD_CMD_DO_TEST_BENCH_WIRING 0
#define SD_CMD_DRIVEN_SIM_VALUE 0
#define SD_CMD_HAS_TRI 1
#define SD_CMD_HAS_OUT 0
#define SD_CMD_HAS_IN 0
#define SD_CMD_CAPTURE 0
#define SD_CMD_DATA_WIDTH 1
#define SD_CMD_RESET_VALUE 0
#define SD_CMD_EDGE_TYPE "NONE"
#define SD_CMD_IRQ_TYPE "NONE"
#define SD_CMD_BIT_CLEARING_EDGE_REGISTER 0
#define SD_CMD_BIT_MODIFYING_OUTPUT_REGISTER 0
#define SD_CMD_FREQ 10000000
#define ALT_MODULE_CLASS_sd_cmd altera_avalon_pio

/*
 * sd_wp_n configuration
 *
 */

#define SD_WP_N_NAME "/dev/sd_wp_n"
#define SD_WP_N_TYPE "altera_avalon_pio"
#define SD_WP_N_BASE 0x00000070
#define SD_WP_N_SPAN 16
#define SD_WP_N_DO_TEST_BENCH_WIRING 0
#define SD_WP_N_DRIVEN_SIM_VALUE 0
#define SD_WP_N_HAS_TRI 0
#define SD_WP_N_HAS_OUT 0
#define SD_WP_N_HAS_IN 1
#define SD_WP_N_CAPTURE 0
#define SD_WP_N_DATA_WIDTH 1
#define SD_WP_N_RESET_VALUE 0
#define SD_WP_N_EDGE_TYPE "NONE"
#define SD_WP_N_IRQ_TYPE "NONE"
#define SD_WP_N_BIT_CLEARING_EDGE_REGISTER 0
#define SD_WP_N_BIT_MODIFYING_OUTPUT_REGISTER 0
#define SD_WP_N_FREQ 10000000
#define ALT_MODULE_CLASS_sd_wp_n altera_avalon_pio

/*
 * buttons configuration
 *
 */

#define BUTTONS_NAME "/dev/buttons"
#define BUTTONS_TYPE "altera_avalon_pio"
#define BUTTONS_BASE 0x00000060
#define BUTTONS_SPAN 16
#define BUTTONS_IRQ 2
#define BUTTONS_DO_TEST_BENCH_WIRING 0
#define BUTTONS_DRIVEN_SIM_VALUE 0
#define BUTTONS_HAS_TRI 0
#define BUTTONS_HAS_OUT 0
#define BUTTONS_HAS_IN 1
#define BUTTONS_CAPTURE 1
#define BUTTONS_DATA_WIDTH 3
#define BUTTONS_RESET_VALUE 0
#define BUTTONS_EDGE_TYPE "ANY"
#define BUTTONS_IRQ_TYPE "EDGE"
#define BUTTONS_BIT_CLEARING_EDGE_REGISTER 0
#define BUTTONS_BIT_MODIFYING_OUTPUT_REGISTER 0
#define BUTTONS_FREQ 10000000
#define ALT_MODULE_CLASS_buttons altera_avalon_pio

/*
 * switches configuration
 *
 */

#define SWITCHES_NAME "/dev/switches"
#define SWITCHES_TYPE "altera_avalon_pio"
#define SWITCHES_BASE 0x00000050
#define SWITCHES_SPAN 16
#define SWITCHES_IRQ 1
#define SWITCHES_DO_TEST_BENCH_WIRING 0
#define SWITCHES_DRIVEN_SIM_VALUE 0
#define SWITCHES_HAS_TRI 0
#define SWITCHES_HAS_OUT 0
#define SWITCHES_HAS_IN 1
#define SWITCHES_CAPTURE 1
#define SWITCHES_DATA_WIDTH 10
#define SWITCHES_RESET_VALUE 0
#define SWITCHES_EDGE_TYPE "ANY"
#define SWITCHES_IRQ_TYPE "EDGE"
#define SWITCHES_BIT_CLEARING_EDGE_REGISTER 0
#define SWITCHES_BIT_MODIFYING_OUTPUT_REGISTER 0
#define SWITCHES_FREQ 10000000
#define ALT_MODULE_CLASS_switches altera_avalon_pio

/*
 * leds configuration
 *
 */

#define LEDS_NAME "/dev/leds"
#define LEDS_TYPE "altera_avalon_pio"
#define LEDS_BASE 0x00000040
#define LEDS_SPAN 16
#define LEDS_DO_TEST_BENCH_WIRING 0
#define LEDS_DRIVEN_SIM_VALUE 0
#define LEDS_HAS_TRI 0
#define LEDS_HAS_OUT 1
#define LEDS_HAS_IN 0
#define LEDS_CAPTURE 0
#define LEDS_DATA_WIDTH 10
#define LEDS_RESET_VALUE 0
#define LEDS_EDGE_TYPE "NONE"
#define LEDS_IRQ_TYPE "NONE"
#define LEDS_BIT_CLEARING_EDGE_REGISTER 0
#define LEDS_BIT_MODIFYING_OUTPUT_REGISTER 0
#define LEDS_FREQ 10000000
#define ALT_MODULE_CLASS_leds altera_avalon_pio

/*
 * cfi_flash configuration
 *
 */

#define CFI_FLASH_NAME "/dev/cfi_flash"
#define CFI_FLASH_TYPE "altera_avalon_cfi_flash"
#define CFI_FLASH_BASE 0x02400000
#define CFI_FLASH_SPAN 4194304
#define CFI_FLASH_SETUP_VALUE 50
#define CFI_FLASH_WAIT_VALUE 100
#define CFI_FLASH_HOLD_VALUE 50
#define CFI_FLASH_TIMING_UNITS "ns"
#define CFI_FLASH_UNIT_MULTIPLIER 1
#define CFI_FLASH_SIZE 4194304
#define ALT_MODULE_CLASS_cfi_flash altera_avalon_cfi_flash

/*
 * tristate_bridge configuration
 *
 */

#define TRISTATE_BRIDGE_NAME "/dev/tristate_bridge"
#define TRISTATE_BRIDGE_TYPE "altera_avalon_tri_state_bridge"
#define ALT_MODULE_CLASS_tristate_bridge altera_avalon_tri_state_bridge

/*
 * pll configuration
 *
 */

#define PLL_NAME "/dev/pll"
#define PLL_TYPE "altera_avalon_pll"
#define PLL_BASE 0x02811000
#define PLL_SPAN 32
#define PLL_ARESET "None"
#define PLL_PFDENA "None"
#define PLL_LOCKED "None"
#define PLL_PLLENA "None"
#define PLL_SCANCLK "None"
#define PLL_SCANDATA "None"
#define PLL_SCANREAD "None"
#define PLL_SCANWRITE "None"
#define PLL_SCANCLKENA "None"
#define PLL_SCANACLR "None"
#define PLL_SCANDATAOUT "None"
#define PLL_SCANDONE "None"
#define PLL_CONFIGUPDATE "None"
#define PLL_PHASECOUNTERSELECT "None"
#define PLL_PHASEDONE "None"
#define PLL_PHASEUPDOWN "None"
#define PLL_PHASESTEP "None"
#define PLL_CONFIG_DONE 0
#define ALT_MODULE_CLASS_pll altera_avalon_pll

/*
 * sdram configuration
 *
 */

#define SDRAM_NAME "/dev/sdram"
#define SDRAM_TYPE "altera_avalon_new_sdram_controller"
#define SDRAM_BASE 0x01800000
#define SDRAM_SPAN 8388608
#define SDRAM_REGISTER_DATA_IN 1
#define SDRAM_SIM_MODEL_BASE 1
#define SDRAM_SDRAM_DATA_WIDTH 16
#define SDRAM_SDRAM_ADDR_WIDTH 12
#define SDRAM_SDRAM_ROW_WIDTH 12
#define SDRAM_SDRAM_COL_WIDTH 8
#define SDRAM_SDRAM_NUM_CHIPSELECTS 1
#define SDRAM_SDRAM_NUM_BANKS 4
#define SDRAM_REFRESH_PERIOD 15.625
#define SDRAM_POWERUP_DELAY 100.0
#define SDRAM_CAS_LATENCY 3
#define SDRAM_T_RFC 70.0
#define SDRAM_T_RP 20.0
#define SDRAM_T_MRD 3
#define SDRAM_T_RCD 20.0
#define SDRAM_T_AC 5.5
#define SDRAM_T_WR 14.0
#define SDRAM_INIT_REFRESH_COMMANDS 2
#define SDRAM_INIT_NOP_DELAY 0.0
#define SDRAM_SHARED_DATA 0
#define SDRAM_SDRAM_BANK_WIDTH 2
#define SDRAM_TRISTATE_BRIDGE_SLAVE ""
#define SDRAM_STARVATION_INDICATOR 0
#define SDRAM_IS_INITIALIZED 1
#define ALT_MODULE_CLASS_sdram altera_avalon_new_sdram_controller

/*
 * lcd_light configuration
 *
 */

#define LCD_LIGHT_NAME "/dev/lcd_light"
#define LCD_LIGHT_TYPE "altera_avalon_pio"
#define LCD_LIGHT_BASE 0x000000c0
#define LCD_LIGHT_SPAN 16
#define LCD_LIGHT_DO_TEST_BENCH_WIRING 0
#define LCD_LIGHT_DRIVEN_SIM_VALUE 0
#define LCD_LIGHT_HAS_TRI 0
#define LCD_LIGHT_HAS_OUT 1
#define LCD_LIGHT_HAS_IN 0
#define LCD_LIGHT_CAPTURE 0
#define LCD_LIGHT_DATA_WIDTH 1
#define LCD_LIGHT_RESET_VALUE 0
#define LCD_LIGHT_EDGE_TYPE "NONE"
#define LCD_LIGHT_IRQ_TYPE "NONE"
#define LCD_LIGHT_BIT_CLEARING_EDGE_REGISTER 0
#define LCD_LIGHT_BIT_MODIFYING_OUTPUT_REGISTER 0
#define LCD_LIGHT_FREQ 10000000
#define ALT_MODULE_CLASS_lcd_light altera_avalon_pio

/*
 * clock_crossing_bridge configuration
 *
 */

#define CLOCK_CROSSING_BRIDGE_NAME "/dev/clock_crossing_bridge"
#define CLOCK_CROSSING_BRIDGE_TYPE "altera_avalon_clock_crossing"
#define CLOCK_CROSSING_BRIDGE_BASE 0x00000000
#define CLOCK_CROSSING_BRIDGE_SPAN 256
#define CLOCK_CROSSING_BRIDGE_UPSTREAM_FIFO_DEPTH 64
#define CLOCK_CROSSING_BRIDGE_DOWNSTREAM_FIFO_DEPTH 16
#define CLOCK_CROSSING_BRIDGE_DATA_WIDTH 32
#define CLOCK_CROSSING_BRIDGE_NATIVE_ADDRESS_WIDTH 6
#define CLOCK_CROSSING_BRIDGE_USE_BYTE_ENABLE 1
#define CLOCK_CROSSING_BRIDGE_USE_BURST_COUNT 0
#define CLOCK_CROSSING_BRIDGE_MAXIMUM_BURST_SIZE 8
#define CLOCK_CROSSING_BRIDGE_UPSTREAM_USE_REGISTER 0
#define CLOCK_CROSSING_BRIDGE_DOWNSTREAM_USE_REGISTER 0
#define CLOCK_CROSSING_BRIDGE_SLAVE_SYNCHRONIZER_DEPTH 3
#define CLOCK_CROSSING_BRIDGE_MASTER_SYNCHRONIZER_DEPTH 3
#define CLOCK_CROSSING_BRIDGE_DEVICE_FAMILY "CYCLONEIII"
#define ALT_MODULE_CLASS_clock_crossing_bridge altera_avalon_clock_crossing

/*
 * seg7 configuration
 *
 */

#define SEG7_NAME "/dev/seg7"
#define SEG7_TYPE "altera_avalon_pio"
#define SEG7_BASE 0x000000d0
#define SEG7_SPAN 16
#define SEG7_DO_TEST_BENCH_WIRING 0
#define SEG7_DRIVEN_SIM_VALUE 0
#define SEG7_HAS_TRI 0
#define SEG7_HAS_OUT 1
#define SEG7_HAS_IN 0
#define SEG7_CAPTURE 0
#define SEG7_DATA_WIDTH 32
#define SEG7_RESET_VALUE 0
#define SEG7_EDGE_TYPE "NONE"
#define SEG7_IRQ_TYPE "NONE"
#define SEG7_BIT_CLEARING_EDGE_REGISTER 0
#define SEG7_BIT_MODIFYING_OUTPUT_REGISTER 0
#define SEG7_FREQ 10000000
#define ALT_MODULE_CLASS_seg7 altera_avalon_pio

/*
 * system library configuration
 *
 */

#define ALT_MAX_FD 32
#define ALT_SYS_CLK TIMER
#define ALT_TIMESTAMP_CLK none

/*
 * Devices associated with code sections.
 *
 */

#define ALT_TEXT_DEVICE       SDRAM
#define ALT_RODATA_DEVICE     SDRAM
#define ALT_RWDATA_DEVICE     SDRAM
#define ALT_EXCEPTIONS_DEVICE ONCHIP_MEM
#define ALT_RESET_DEVICE      ONCHIP_MEM


#endif /* __SYSTEM_H_ */
