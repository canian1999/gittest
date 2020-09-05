
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

	#define KEY_MasterAddressSet 0x0C   //ͬʱ�������Ұ���
	#define KEY_SlaveAddressSet  0x30   //ͬʱ���¼Ӽ�����

	//KEY_SWIMA: �л�����������������������ʽ���������ﴫ����Ϊ��������Ϊ���������½�ʽ���������ﴫ����Ϊ��������Ϊ����

	#define KEY_STOP              0xF0    //����KEY8
	#define KEY_QUERY             0x0F    //����KEY1
	#define KEY_RESET             0xAA    //����KEY1
	#define KEY_DEBUG             0xA1    //����KEY3
	#define KEY_DEBUGV            0xA2    //����KEY3


	#define KEY_SETSENSOR      0x03    //����KEY_PLUS  KEY4

	#define KEY_CSELL          0xE0    //���Ƽ�����
    #define KEY_CSELR          0x70    //���Ƽ�����
    #define KEY_CPLUS          0x0E    //�Ӻż�����
	#define KEY_CMINUS         0x07    //���ż�����

	#define KEY_NULL           0x00

	#define KEY_CurrSet        0x17


    #define KEYJIADELAY        50
    #define KEYDELAY           120     //20ms�жϣ�20 * 120 = 2400ms
	#define KEYLOOP            10



	void KeyScan(void);  

#endif 
