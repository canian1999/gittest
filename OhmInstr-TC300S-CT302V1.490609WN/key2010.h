
#ifndef _AVR_KEY_H_
#define _AVR_KEY_H_ 1
	#include "stm32f10x_lib.h"

    //74LS165
    #define SET_KEYCK          GPIOB->BSRR  |= GPIO_Pin_0
    #define CLR_KEYCK          GPIOB->BRR   |= GPIO_Pin_0

    #define SET_KEYPL          GPIOB->BSRR  |= GPIO_Pin_1
    #define CLR_KEYPL          GPIOB->BRR   |= GPIO_Pin_1

    #define KEY_RUN            0x01   //KEY8
    #define KEY_SETUP          0x02   //KEY7
	#define KEY_SELR           0x08   //KEY6
	#define KEY_SELL           0x04   //KEY5
	#define KEY_PLUS           0x10   //KEY4
	#define KEY_MINUS          0x20   //KEY3
	#define KEY_ENTER          0x40   //KEY2
	#define KEY_SWIMA          0x80   //KEY1

	#define KEY_MasterAddressSet 0x0C   //同时按下左右按键
	#define KEY_SlaveAddressSet  0x30   //同时按下加减按键

	//KEY_SWIMA: 切换主、辅传感器；气流上升式烤房以下棚传感器为主、上棚为辅，气流下降式烤房以上棚传感器为主、下棚为辅；

	#define KEY_STOP              0xF0    //长按KEY8
	#define KEY_QUERY             0x0F    //长按KEY1
	#define KEY_RESET             0xAA    //长按KEY1
	#define KEY_DEBUG             0xA1    //长按KEY3
	#define KEY_DEBUGV            0xA2    //长按KEY3


	#define KEY_SETSENSOR      0x03    //长按KEY_PLUS  KEY4

	#define KEY_CSELL          0xE0    //左移键连击
    #define KEY_CSELR          0x70    //右移键连击
    #define KEY_CPLUS          0x0E    //加号键连击
	#define KEY_CMINUS         0x07    //减号键连击

	#define KEY_NULL           0x00

	#define KEY_CurrSet        0x17


    #define KEYJIADELAY        50
    #define KEYDELAY           120     //20ms中断，20 * 120 = 2400ms
	#define KEYLOOP            10



	void KeyScan(void);  

#endif 
