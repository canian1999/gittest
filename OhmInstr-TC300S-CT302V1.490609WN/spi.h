#ifndef __DK_SPI_h__
#define __DK_SPI_h__

#define nSEL_H() 						GPIOB->BSRR  |= GPIO_Pin_9
#define nSEL_L() 						GPIOB->BRR  |= GPIO_Pin_9

//#define SCK_H()							GPIOA->BSRR  |= GPIO_Pin_5
//#define SCK_L()							GPIOA->BRR  |= GPIO_Pin_5

//#define SDI_H()							GPIOA->BSRR  |= GPIO_Pin_7
//#define SDI_L()							GPIOA->BRR  |= GPIO_Pin_7

#define SDN_H()							GPIOB->BSRR  |= GPIO_Pin_4
#define SDN_L()							GPIOB->BRR  |= GPIO_Pin_4

//#define Get_SDO()					  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)
#define Get_NIRQ()				        GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)

unsigned char SPICmd8bit(unsigned char WrPara);
unsigned char SPIRead(unsigned char adr);
unsigned char SPIRead8bit(void);
void SPIWrite(unsigned char adr, unsigned char WrPara);
void SPIBurstRead(unsigned char adr, unsigned char *ptr,unsigned char length);
void BurstWrite(unsigned char adr, unsigned char *ptr, unsigned char length);

#endif







