// --------------------------------------------------------------------
// Copyright (c) 2007 by Terasic Technologies Inc. 
// --------------------------------------------------------------------
//
// Permission:
//
//   Terasic grants permission to use and modify this code for use
//   in synthesis for all Terasic Development Boards and Altera Development 
//   Kits made by Terasic.  Other use of this code, including the selling 
//   ,duplication, or modification of any portion is strictly prohibited.
//
// Disclaimer:
//
//   This VHDL/Verilog or C/C++ source code is intended as a design reference
//   which illustrates how these types of functions can be implemented.
//   It is the user's responsibility to verify their design for
//   consistency and functionality through the use of formal
//   verification methods.  Terasic provides no warranty regarding the use 
//   or functionality of this code.
//
// --------------------------------------------------------------------
//           
//                     Terasic Technologies Inc
//                     356 Fu-Shin E. Rd Sec. 1. JhuBei City,
//                     HsinChu County, Taiwan
//                     302
//
//                     web: http://www.terasic.com/
//                     email: support@terasic.com
//
// --------------------------------------------------------------------

#ifndef SD_CARD_DRIVER_H_
#define SD_CARD_DRIVER_H_

#include "..\terasic_lib\terasic_includes.h"
#include "alt_types.h"  // alt_u32
#include "io.h"
#include "system.h"


#define xSD_4BIT_MODE

// direction control
#define SD_CMD_IN   IOWR_ALTERA_AVALON_PIO_DIRECTION(SD_CMD_BASE, ALTERA_AVALON_PIO_DIRECTION_INPUT)
#define SD_CMD_OUT  IOWR_ALTERA_AVALON_PIO_DIRECTION(SD_CMD_BASE, ALTERA_AVALON_PIO_DIRECTION_OUTPUT)
#define SD_DAT_IN   IOWR_ALTERA_AVALON_PIO_DIRECTION(SD_DAT_BASE, ALTERA_AVALON_PIO_DIRECTION_INPUT)
#define SD_DAT_OUT  IOWR_ALTERA_AVALON_PIO_DIRECTION(SD_DAT_BASE, ALTERA_AVALON_PIO_DIRECTION_OUTPUT)
//  SD Card Output High/Low
#define SD_CMD_LOW  IOWR_ALTERA_AVALON_PIO_DATA(SD_CMD_BASE, 0)
#define SD_CMD_HIGH IOWR_ALTERA_AVALON_PIO_DATA(SD_CMD_BASE, 1)
#define SD_CLK_LOW  IOWR_ALTERA_AVALON_PIO_DATA(SD_CLK_BASE, 0)
#define SD_CLK_HIGH IOWR_ALTERA_AVALON_PIO_DATA(SD_CLK_BASE, 1)
#define SD_DAT_LOW  IOWR_ALTERA_AVALON_PIO_DATA(SD_DAT_BASE, 0)
#define SD_DAT_HIGH IOWR_ALTERA_AVALON_PIO_DATA(SD_DAT_BASE, 1)
#define SD_DAT_WRITE(data4) IOWR_ALTERA_AVALON_PIO_DATA(SD_DAT_BASE, data4)
//  SD Card Input Read
#define SD_TEST_CMD  IORD_ALTERA_AVALON_PIO_DATA(SD_CMD_BASE)
#define SD_TEST_DAT  IORD_ALTERA_AVALON_PIO_DATA(SD_DAT_BASE)



bool SD_card_init(void);
bool SD_read_block(alt_u32 block_number, alt_u8 *buff); // richard add, return 0: success
bool SD_GetCSD(alt_u8 szCSD[], alt_u8 len);
bool SD_GetCID(alt_u8 szCID[], alt_u8 len);




#endif /*SD_CARD_DRIVER_H_*/
