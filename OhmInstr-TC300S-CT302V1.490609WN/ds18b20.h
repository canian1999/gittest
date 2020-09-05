#ifndef _DS18B20_H_
#define _DS18B20_H_
#include "stm32f10x_lib.h"
#include "stm32f10x_gpio.h"

#define DS_PORT   GPIOB
#define TEMP_SAMP_CH0   12
#define TEMP_SAMP_CH1   11
#define TEMP_SAMP_CH2   10
#define TEMP_SAMP_CH3   13
#define DS_RCC_PORT  RCC_APB2Periph_GPIOB
#define  SkipROM    0xCC     //����ROM
#define  StartConvert    0x44  //��ʼ�¶�ת�������¶�ת���ڼ����������0��ת�����������1
#define  ReadScratchpad  0xBE  //���ݴ�����9���ֽ�

unsigned char ucStartRdTmp(unsigned int *ptmp);

#endif   /*end of ifndef */
