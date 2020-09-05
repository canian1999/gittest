
#ifndef _AVR_YUYIN_H_
#define _AVR_YUYIN_H_

   
    //aP89170 COMMAND
    #define CMD_PUP1          0xC5
    #define CMD_PUP2          0x8D
    #define CMD_PDN1          0xE1
	#define CMD_PDN2          0xA9
    #define CMD_PLAY          0x55
	#define CMD_STATUS        0xE3
    #define CMD_PAUSE         0x39
    #define CMD_RESUME        0x1D
    #define CMD_PREFETCH      0x71   


    //aP89170
    #define SET_YUSCK         GPIO_SetBits(GPIOB,GPIO_Pin_6)
    #define CLR_YUSCK         GPIO_ResetBits(GPIOB,GPIO_Pin_6)

    #define SET_YUCS          GPIO_SetBits(GPIOB,GPIO_Pin_7)
    #define CLR_YUCS          GPIO_ResetBits(GPIOB,GPIO_Pin_7)

    #define SET_YUDA          GPIO_SetBits(GPIOB,GPIO_Pin_5)
    #define CLR_YUDA          GPIO_ResetBits(GPIOB,GPIO_Pin_5)

	#define VOICEBUSY_IN      GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)
    #define VOICEBUSY_PIN     GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)

	//定义语音地址
    #define   WELCOME         0x2E    //46:欢迎辞

	#define   OVER_TIME       0x2D    //45:稳温超时

    #define   STOP_RUN        0x2B    //43:系统关闭
    #define   START_RUN       0x2A    //42:开始运行

    #define   SENSOR_WATER    0x21    //请检查传感器和水壶 

	#define   WET_LOW         0x1F    //31:湿球温度偏低
    #define   WET_HIGH        0x1E    //30:湿球温度偏高
	#define   DRY_LOW         0x1D    //29:干球温度偏低
	#define   DRY_HIGH        0x1C    //28:干球温度偏高

	#define   SENSOR_ERR      0x1B    //27:传感器故障

    #define   VOL_LOW         0x1A    //26:电压偏低
	#define   VOL_HIGH        0x19    //25:电压偏高，请关闭系统电源(将来与电压偏低合成，电压异常)

    #define   FENGJI_NOCUR    0x18    //24:风机无电流

	#define   FENGJI_OVER     0x16    //22:风机过载
	#define   FEGNJI_LACK     0x15    //21:风机缺相

    #define   POWER_LACK              //:电源缺相

extern	void InitVoiceIO(void);
extern	void VoicePlayAll(void);
extern	void VoicePlay(unsigned char gaddr);

#endif 
