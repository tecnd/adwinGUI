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
// --------------------------------------------------------------------
// Copyright (c) 2005 by Terasic Technologies Inc. 
// --------------------------------------------------------------------
//
// Permission:
//
//   Terasic grants permission to use this code for all Terasic Development Boards 
//   and Altera Development Kits made by Terasic.  
//   Other use of this code, including the selling 
//   ,duplication, or modification of any portion is strictly prohibited.
//
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
//
//---------------------------------------------------------------------------

#include <unistd.h>  // usleep
#include <stdio.h>
#include "SDCardDriver.h"
//#include "..\terasic_lib\terasic_includes.h"
//#include "..\terasic_lib\debug.h"


#ifdef DEBUG_SDCARD
    #define SDCARD_DEBUG(x)    {DEBUG(("[SD]")); DEBUG(x);}  
#else
    #define SDCARD_DEBUG(x)
#endif


#define DEBUG_SDCARD_HEX //DEBUG_HEX_PRINTF


alt_u8 gszCSD[17];
alt_u8 gszCID[17];
bool gbActive = FALSE;

//-------------------------------------------------------------------------
void Ncr(void);
void Ncc(void);
alt_u8 send_cmd(alt_u8 *in);
alt_u8 send_cmd(alt_u8 *);
alt_u8 response_R(alt_u8 s);
//-------------------------------------------------------------------------
alt_u8 read_status;
alt_u8 response_buffer[20];
alt_u8 RCA[2];
alt_u8 cmd_buffer[5];
const alt_u8 cmd0[5]   = {0x40,0x00,0x00,0x00,0x00};
const alt_u8 cmd55[5]  = {0x77,0x00,0x00,0x00,0x00};
const alt_u8 cmd2[5]   = {0x42,0x00,0x00,0x00,0x00};
const alt_u8 cmd3[5]   = {0x43,0x00,0x00,0x00,0x00};
const alt_u8 cmd7[5]   = {0x47,0x00,0x00,0x00,0x00};
const alt_u8 cmd9[5]   = {0x49,0x00,0x00,0x00,0x00};
const alt_u8 cmd10[5]  = {0x4a,0x00,0x00,0x00,0x00};  // richard add
const alt_u8 cmd16[5]  = {0x50,0x00,0x00,0x02,0x00}; // block length = 512 byte
const alt_u8 cmd17[5]  = {0x51,0x00,0x00,0x00,0x00};
const alt_u8 acmd6[5]  = {0x46,0x00,0x00,0x00,0x02};  // bus width, 4-bits
const alt_u8 acmd41[5] = {0x69,0x0f,0xf0,0x00,0x00};  // 0x76 = 41 + 0x40
const alt_u8 acmd42[5] = {0x6A,0x00,0x00,0x00,0x00};  // richard add, SET_CLR_CARD_DETECT
const alt_u8 acmd51[5] = {0x73,0x00,0x00,0x00,0x00};
//-------------------------------------------------------------------------
void Ncr(void)
{
  SD_CMD_IN;
  SD_CLK_LOW;
  SD_CLK_HIGH;
  SD_CLK_LOW;
  SD_CLK_HIGH;
} 
//-------------------------------------------------------------------------
void Ncc(void)
{
  int i;
  for(i=0;i<8;i++)
  {
    SD_CLK_LOW;
    SD_CLK_HIGH;
  }
}




void dump_CSD(alt_u8 szCSD[]){
   SDCARD_DEBUG(("SD-CARD CSD:...\r\n"));
    
}

void dump_CID(alt_u8 szCID[]){
    int i = 1;
   SDCARD_DEBUG(("SD-CARD CID:\r\n"));
   SDCARD_DEBUG(("  Manufacturer ID(MID):%02Xh\r\n", szCID[i+0]));
   SDCARD_DEBUG(("  OEM/Application ID(OLD):%02X%02Xh\r\n", szCID[i+2], szCID[i+1]));
   SDCARD_DEBUG(("  Product Name(PNM):%C%C%C%C%C\r\n", szCID[i+3], szCID[i+4], szCID[i+5], szCID[i+6], szCID[i+7]));
   SDCARD_DEBUG(("  Product Revision:%02Xh\r\n", szCID[i+8]));
   SDCARD_DEBUG(("  Serail Number(PSN):%02X%02X%02X%02Xh\r\n", szCID[i+9], szCID[i+10], szCID[i+11], szCID[i+12]));
   SDCARD_DEBUG(("  Manufacturere Date Code(MDT):%01X%02Xh\r\n", szCID[i+13] & 0x0F, szCID[i+14]));
   SDCARD_DEBUG(("  CRC-7 Checksum(CRC7):%02Xh\r\n", szCID[i+15] >> 1));
    
}

bool SD_GetCSD(alt_u8 szCSD[], alt_u8 len){
    if (!gbActive)
        return FALSE;
    if (len > 16)
        len = 16;        
    memcpy(szCSD, &gszCSD[1], len);         
    return TRUE;        
}

bool SD_GetCID(alt_u8 szCID[], alt_u8 len){
    if (!gbActive)
        return FALSE;
    if (len > 16)
        len = 16;        
    memcpy(szCID, &gszCID[1], len);         
    return TRUE;        
}





//-------------------------------------------------------------------------
bool SD_card_init(void)
{
    alt_u8 x,y;
    
    // richard add:  pull-high DAT3 to enter SD mode?
#ifndef SD_4BIT_MODE    
  //  SD_DAT3_OUT;
   // SD_DAT3_HIGH;
#endif  
    SDCARD_DEBUG(("--- Power On, Card Identification Mode, Idle State\r\n"));
    SDCARD_DEBUG(("default 1-bit mode\r\n"));
  
    usleep(74*10);
    
    SD_CMD_OUT;
    SD_DAT_IN;
    SD_CLK_HIGH;
    SD_CMD_HIGH;
    SD_DAT_LOW;
    
    gbActive = FALSE;
    read_status=0;
    for(x=0;x<40;x++)
        Ncr();
    for(x=0;x<5;x++)
        cmd_buffer[x]=cmd0[x];
    y = send_cmd(cmd_buffer);
    SDCARD_DEBUG(("CMD0[GO_IDLE_STATE]\r\n"));
    do
    {
        // issue cmd55 & wait response 
      for(x=0;x<40;x++);
        Ncc();
      for(x=0;x<5;x++)
        cmd_buffer[x]=cmd55[x];
      y = send_cmd(cmd_buffer);
      SDCARD_DEBUG(("CMD55[APP_CMD]\r\n"));
      Ncr();
      if(response_R(1)>1){ //response too long or crc error
        SDCARD_DEBUG(("response error for CMD55\r\n"));
        return FALSE;
      }          
      Ncc();
        
        // issue acmd41 & wait response 
      for(x=0;x<5;x++)
        cmd_buffer[x]=acmd41[x];
      y = send_cmd(cmd_buffer);
      SDCARD_DEBUG(("ACMD41[SD_APP_OP_COND]\r\n"));
        Ncr();      
    } while(response_R(3)==1);
    
    
    // issue cmd2 & wait response 
    Ncc();
    for(x=0;x<5;x++)
        cmd_buffer[x]=cmd2[x];
    y = send_cmd(cmd_buffer);
    SDCARD_DEBUG(("CMD2[ALL_SEND_CID]\r\n"));
    Ncr();
    if(response_R(2)>1){
        SDCARD_DEBUG(("CMD2 fail\r\n"));
        return FALSE;
    }        
        
    SDCARD_DEBUG(("--- Power On, Card Identification Mode, Identification State\r\n"));
        
    // issue cmd3 & wait response, finally get RCA 
    Ncc();
    for(x=0;x<5;x++)
        cmd_buffer[x]=cmd3[x];
    y = send_cmd(cmd_buffer);
    SDCARD_DEBUG(("CMD3[SEND_RELATIVE_ADDR]\r\n"));
    Ncr();
    if(response_R(6)>1){
        SDCARD_DEBUG(("CMD3 fail\r\n"));
        return FALSE;
    }        
        
    RCA[0]=response_buffer[1];
    RCA[1]=response_buffer[2];
    
     
    
    // above is Card Identification Mode
    //*************** now, wer are in Data Transfer Mode ********************************/
    //### Standby-by state in Data-transfer mode
    
    SDCARD_DEBUG(("--- enter data-transfer mode, Standy-by stater\n"));
    // issue cmd9 with given RCA & wait response 
    Ncc();
    for(x=0;x<5;x++)
        cmd_buffer[x]=cmd9[x];
    cmd_buffer[1] = RCA[0];
    cmd_buffer[2] = RCA[1];  
    y = send_cmd(cmd_buffer);
    SDCARD_DEBUG(("CMD9[SEND_CSD]\r\n"));
    Ncr();
    if(response_R(2)>1){
        SDCARD_DEBUG(("CMD9 fail\r\n"));
        return FALSE;
    }        
    memcpy(gszCSD,&response_buffer[0] , sizeof(gszCSD));    // richard add
    //DEBUG_SDCARD("\r\nCSD Hex:\r\n");
    //DEBUG_SDCARD_HEX(gszCSD, sizeof(gszCSD));
    //dump_CSD(gszCSD);
    
    // richard add  (query card identification)  
    Ncc();
    for(x=0;x<5;x++)
        cmd_buffer[x]=cmd10[x];
    cmd_buffer[1] = RCA[0];
    cmd_buffer[2] = RCA[1];  
    y = send_cmd(cmd_buffer);
    SDCARD_DEBUG(("CMD10[SEND_CID]\r\n"));
    Ncr();
    if(response_R(2)>1){
        SDCARD_DEBUG(("CMD10 fail\r\n"));
        return FALSE;
    }        
    memcpy(gszCID,&response_buffer[0] , sizeof(gszCID));    // richard add
    //DEBUG_SDCARD("\r\nCID Hex:\r\n");
    //DEBUG_SDCARD_HEX(gszCID, sizeof(gszCID));
    dump_CID(gszCID);

    // can issue cmd 4, 9, 10, in (stdandby state)         
         
    // issue cmd9 with given RCA & wait response
    
  
    
    
    
    // richard: issue cmd7 to enter transfer state
    // cmd7: toggle between Standy-by and Trasfer State 
    Ncc();
    for(x=0;x<5;x++)
        cmd_buffer[x]=cmd7[x];
    cmd_buffer[1] = RCA[0];
    cmd_buffer[2] = RCA[1];
    y = send_cmd(cmd_buffer);
    SDCARD_DEBUG(("CMD7[SELECT/DESELECT_CARD], select card\r\n"));
    Ncr();
    if(response_R(1)>1){
        SDCARD_DEBUG(("CMD7 fail\r\n"));
        return FALSE;
    }        
        
    //### Transfer state in Data-transfer mode     
    SDCARD_DEBUG(("--- enter data-transfer mode, Transfer state\r\n"));
    
    
  
    
       
         
    // issue cmd16 (select a block length) & wait response 
    Ncc();
    for(x=0;x<5;x++)
        cmd_buffer[x]=cmd16[x];
    y = send_cmd(cmd_buffer);  
    SDCARD_DEBUG(("CMD16[SET_BLOCK_LENGTH], 512 bytes\r\n"));
    Ncr();
    if(response_R(1)>1){
        SDCARD_DEBUG(("CMD16 fail\r\n"));
        return FALSE;
    }        
    
#ifdef SD_4BIT_MODE    
    // richard add: set bus width
    // Note. This command is valid only in "transfer state", i.e. after CMD7 is issued

 
    Ncc();
    for(x=0;x<5;x++)
        cmd_buffer[x]=cmd55[x];
    cmd_buffer[1] = RCA[0]; // note. remember to fill RCA
    cmd_buffer[2] = RCA[1];
    y = send_cmd(cmd_buffer);  
    SDCARD_DEBUG(("ACM55[APP_CMD]\r\n"));
    Ncr();
    if(response_R(1)>1){
        SDCARD_DEBUG(("CMD55 fail\r\n"));
        return FALSE;
    }  
        
    
    Ncc();
    for(x=0;x<5;x++)
        cmd_buffer[x]=acmd6[x];
    y = send_cmd(cmd_buffer);  
    SDCARD_DEBUG(("ACMD6[SET_BUS_WIDTH], 4-bit\r\n"));
    Ncr();
    if(response_R(1)>1){
        SDCARD_DEBUG(("ACMD6 fail\r\n"));
        return FALSE;
    }  
    
    
    //
    Ncc();
    for(x=0;x<5;x++)
        cmd_buffer[x]=cmd55[x];
    cmd_buffer[1] = RCA[0]; // note. remember to fill RCA
    cmd_buffer[2] = RCA[1];
    y = send_cmd(cmd_buffer);  
    SDCARD_DEBUG(("ACM55[APP_CMD]\r\n"));
    Ncr();
    if(response_R(1)>1){
        SDCARD_DEBUG(("CMD55 fail\r\n"));
        return FALSE;
    }  
        
    
    Ncc();
    for(x=0;x<5;x++)
        cmd_buffer[x]=acmd42[x];
    y = send_cmd(cmd_buffer);  
    SDCARD_DEBUG(("ACMD42[SET_CLR_CARD_DETECT], 4-bit\r\n"));
    Ncr();
    if(response_R(1)>1){
        SDCARD_DEBUG(("ACMD42 fail\r\n"));
        return FALSE;
    }      
    
   
    
#endif       
     
    SDCARD_DEBUG(("SD_card_init success\r\n"));
    read_status =1; //sd card ready
    gbActive = TRUE;
    return TRUE;
}


bool SD_read_block(alt_u32 block_number, alt_u8 *buff)
{
  // buffer size muse be 512 byte  
  alt_u8 c=0;
  alt_u32  i,addr; //j,addr;
  int try = 0;
  const int max_try = 5000;
  
    // issue cmd17 for 'Single Block Read'. parameter: block address 
    {
      Ncc();
      addr = block_number * 512;
      cmd_buffer[0] = cmd17[0]; // CMD17: Read Single Block
      cmd_buffer[1] = (addr >> 24 ) & 0xFF; // MSB
      cmd_buffer[2] = (addr >> 16 ) & 0xFF;
      cmd_buffer[3] = (addr >> 8 ) & 0xFF;
      cmd_buffer[4] = addr & 0xFF; // LSB
      send_cmd(cmd_buffer); 
      Ncr();
    } 
    
    // get response
    while(1)
    {
      SD_CLK_LOW;
      SD_CLK_HIGH;
//      if(!(SD_TEST_DAT))
      if((SD_TEST_DAT & 0x01) == 0x00) // check bit0 
        break;
      if (try++ > max_try)
        return FALSE;        
    }
    
    // read data (512byte = 1 block)
    for(i=0;i<512;i++)
    {
      alt_u8 j;
      c = 0; // richard add
#ifdef SD_4BIT_MODE
      for(j=0;j<2;j++)
      {
        SD_CLK_LOW;
        SD_CLK_HIGH;
        c <<= 4; 
        c |= (SD_TEST_DAT & 0x0F);
      } 
#else      
      for(j=0;j<8;j++)
      {
        SD_CLK_LOW;
        SD_CLK_HIGH;
        c <<= 1; 
        if(SD_TEST_DAT & 0x01)  // check bit0
        c |= 0x01;
      } 
#endif  

        
      *buff=c;
       buff++;
    }

    //
    for(i=0; i<16; i++)
    {
        SD_CLK_LOW;
        SD_CLK_HIGH;
    }
  read_status = 1;  //SD data next in
  return TRUE;
}


//-------------------------------------------------------------------------
alt_u8 response_R(alt_u8 s)
{
  alt_u8 a=0,b=0,c=0,r=0,crc=0;
  alt_u8 i,j=6,k;
  while(1)
  {
    SD_CLK_LOW;
    SD_CLK_HIGH;
    if(!(SD_TEST_CMD))
        break;
    if(crc++ >100)
        return 2;
  } 
  crc =0;
  if(s == 2)
    j = 17;

  for(k=0; k<j; k++)
  {
    c = 0;
    if(k > 0)                      //for crc culcar
        b = response_buffer[k-1];    
    for(i=0; i<8; i++)
    {
      SD_CLK_LOW;
      if(a > 0)
      c <<= 1; 
      else
      i++; 
      a++; 
      SD_CLK_HIGH;
      if(SD_TEST_CMD)
      c |= 0x01;
      if(k > 0)
      {
        crc <<= 1;
        if((crc ^ b) & 0x80)
        crc ^= 0x09;
        b <<= 1;
        crc &= 0x7f;
      }
    }
    if(s==3)
    { 
      if( k==1 &&(!(c&0x80)))
      r=1;
    }
    response_buffer[k] = c;
  }
  if(s==1 || s==6)
  {
    if(c != ((crc<<1)+1))
    r=2;
  } 
  return r; 
}
//-------------------------------------------------------------------------
alt_u8 send_cmd(alt_u8 *in)
{
  int i,j;
  alt_u8 b,crc=0;
  SD_CMD_OUT;
  for(i=0; i < 5; i++)
  {
    b = in[i];
    for(j=0; j<8; j++)
    {
      SD_CLK_LOW;
      if(b&0x80)
      SD_CMD_HIGH;
      else
      SD_CMD_LOW; 
      crc <<= 1;
      SD_CLK_HIGH;
      if((crc ^ b) & 0x80)
      crc ^= 0x09;
      b<<=1;
    } 
    crc &= 0x7f; 
  }  
  crc =((crc<<1)|0x01);
  b = crc; 
  for(j=0; j<8; j++)
  {
    SD_CLK_LOW;
    if(crc&0x80)
    SD_CMD_HIGH;
    else
    SD_CMD_LOW; 
    SD_CLK_HIGH;
    crc<<=1;
  }    
  return b;   
}



