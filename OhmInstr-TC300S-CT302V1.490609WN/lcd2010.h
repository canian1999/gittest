#ifndef _LCD_H_
#define _LCD_H_
#include "stm32f10x_lib.h"
#include "stm32f10x_gpio.h"


//ht1621 COMMAND
#define CMDOFF      0x00
#define CMDON       0x01
#define CMDLCDOFF   0x02
#define CMDLCDON    0x03
#define CMDB3C4     0x29      


//#define  CS_1       0x08

#define  CS_1       0x01    //01
#define  CS_2       0x02    //02
#define  CS_3       0x04    //04
#define  CS_4       0x08    //08
#define  CS_5       0x40    //40
#define  CS_6       0x20    //20
#define  CS_7       0x10    //10
#define  CS_8       0x80    //80

#define  CS_NO      0x00 


//ht1621 时钟信号
#define SET_LCDCK      GPIOC->BSRR |= GPIO_Pin_7   //PC7 FT 兼容上拉到5V
#define CLR_LCDCK      GPIOC->BRR  |= GPIO_Pin_7

//ht1621 数据线
#define SET_LCDDA      GPIOC->BSRR |= GPIO_Pin_8   //DATA对应引脚,FT兼容上拉到5V
#define CLR_LCDDA      GPIOC->BRR  |= GPIO_Pin_8

//ht1621 背光控制线
#define CLR_LCDLED     GPIOC->BRR |= GPIO_Pin_9 
#define SET_LCDLED     GPIOC->BSRR  |= GPIO_Pin_9   //LCD backLight

#define REVERSE_LCDLED GPIO_WriteBit(GPIOC, GPIO_Pin_9, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_9)))


//74LS164
#define SET_LCDCSCK    GPIOA->BSRR |= GPIO_Pin_11
#define CLR_LCDCSCK    GPIOA->BRR  |= GPIO_Pin_11

#define SET_LCDCSDA    GPIOA->BSRR  |= GPIO_Pin_12
#define CLR_LCDCSDA    GPIOA->BRR  |= GPIO_Pin_12


void lcdwbit(unsigned char n);
void LcdSendId(unsigned char ID);
void LcdSendAddress(unsigned char address);
void lcdwd(unsigned char address,unsigned char dat);
void LcdSendByte(unsigned char dat);
void LcdSendHalfByte(unsigned char dat);
void lcdwc(unsigned char cs, unsigned char cmdcode);  //送控制字 
void lcdreset(void);                    //初始化
void vdInitLCD(void);
void vdDispBrush(void);

void vdDrv164(unsigned char dat);



#endif   /*end of ifndef */










