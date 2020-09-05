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
#define  SkipROM    0xCC     //跳过ROM
#define  StartConvert    0x44  //开始温度转换，在温度转换期间总线上输出0，转换结束后输出1
#define  ReadScratchpad  0xBE  //读暂存器的9个字节

unsigned char ucStartRdTmp(unsigned int *ptmp);

#endif   /*end of ifndef */
