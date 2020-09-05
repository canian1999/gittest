// ========================================================
/// @file       SPI.c
/// @brief      Software simulation SPI performance function
/// @version    V1.0
/// @date       2014/05/04
/// @company    HR_WT.
/// @website    http://www.HR_WT.com
/// @author     ISA
/// @mobile     
/// @tel        
// ========================================================
#include "stm32f10x_lib.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "system.h"
#include "spi.h"
unsigned char u8Test = 1;
unsigned char u8Test1 = 1;
unsigned char u8Test2;

/**********************************************************
**Name:     SPICmd8bit
**Function: SPI Write one byte
**Input:    WrPara
**Output:   none
**note:     use for burst mode
**********************************************************/
unsigned char SPICmd8bit(u8 WrPara)
{
  //u8 bitcnt;  
  //nSEL_L();
  //SCK_L();
  
  //for(bitcnt=8; bitcnt!=0; bitcnt--)
  //{
   // SCK_L();
   // if(WrPara&0x80)
      //SDI_H();
    //else
      //SDI_L();
		//_delay_us1(u8Test);
    //SCK_H();
		//_delay_us1(u8Test);
    //WrPara <<= 1;
 // }
  //SCK_L();
  //SDI_H();
	//while(SPI_I2S_GetITStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1,WrPara);

//?????????????,??

	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) == RESET);

	return SPI_I2S_ReceiveData(SPI1);
}

/**********************************************************
**Name:     SPIRead8bit
**Function: SPI Read one byte
**Input:    None
**Output:   result byte
**Note:     use for burst mode
**********************************************************/
unsigned char SPIRead8bit(void)
{
 //u8 RdPara = 0;
 //u8 bitcnt;
 
  //nSEL_L();
  //SDI_H();                                                 //Read one byte data from FIFO, MOSI hold to High
  //for(bitcnt=8; bitcnt!=0; bitcnt--)
  //{
    //SCK_L();
		//_delay_us1(u8Test1);
   // RdPara <<= 1;
    //SCK_H();
		//_delay_us1(0);
    //if(Get_SDO())
      //RdPara |= 0x01;
    //else
      //RdPara |= 0x00;
		//_delay_us1(0);
  //}
  //SCK_L();
  return SPICmd8bit(0);
}

/**********************************************************
**Name:     SPIRead
**Function: SPI Read CMD
**Input:    adr -> address for read
**Output:   None
**********************************************************/
unsigned char SPIRead(unsigned char adr)
{
  unsigned char tmp; 
	nSEL_L();	
  SPICmd8bit(adr);                                         //Send address first
  tmp = SPIRead8bit();  
  nSEL_H();
  return(tmp);
}

/**********************************************************
**Name:     SPIWrite
**Function: SPI Write CMD
**Input:    u8 address & u8 data
**Output:   None
**********************************************************/
void SPIWrite(unsigned char adr, unsigned char WrPara)  
{
	
	nSEL_L();						
 
	SPICmd8bit(adr|0x80);		 //写入地址 
	SPICmd8bit(WrPara);//写入数据
	
	//SCK_L();
  //SDI_H();
  nSEL_H();
}
/**********************************************************
**Name:     SPIBurstRead
**Function: SPI burst read mode
**Input:    adr-----address for read
**          ptr-----data buffer point for read
**          length--how many bytes for read
**Output:   None
**********************************************************/
void SPIBurstRead(unsigned char adr, unsigned char *ptr, unsigned char length)
{
  u8 i;
  if(length<=1)                                            //length must more than one
    return;
  else
  {
    //SCK_L(); 
    nSEL_L();
    SPICmd8bit(adr); 
    for(i=0;i<length;i++)
    ptr[i] = SPIRead8bit();
    nSEL_H();  
  }
}

/**********************************************************
**Name:     SPIBurstWrite
**Function: SPI burst write mode
**Input:    adr-----address for write
**          ptr-----data buffer point for write
**          length--how many bytes for write
**Output:   none
**********************************************************/
void BurstWrite(unsigned char adr, unsigned char *ptr, unsigned char length)
{ 
  u8 i;

  if(length<=1)                                            //length must more than one
    return;
  else  
  {   
    //SCK_L();
    nSEL_L();        
    SPICmd8bit(adr|0x80);
    for(i=0;i<length;i++)
			SPICmd8bit(ptr[i]);
    nSEL_H();  
  }
}





