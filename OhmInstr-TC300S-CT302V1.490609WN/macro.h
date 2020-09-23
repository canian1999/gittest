#ifndef _MACRO_H_
#define _MACRO_H_ 1

    //定义烤房类型
    #define  OVENTYPE     0      //0: 气流上升式；1：气流下降式

    #define  TRUE         1
	#define  FALSE        0  

	#define     UPSENSOR        0x00      //气流下降式烤房
	#define     DWSENSOR        0x01      //气流上升式烤房

	//传感器值存放缓冲区定义
	#define  DNDRY       1  //0   //下棚干温 
	#define  DNWET       2  //1   //下棚湿温
	#define  UPDRY       3  //2   //上棚干温 
  #define  UPWET       0  //3   //上棚湿温

	#define ACV_SAMP_CH0 ADC_Channel_0
	#define ACC_SAMP_CH0 ADC_Channel_10
	#define ACC_SAMP_CH1 ADC_Channel_11
	#define ACC_SAMP_CH2 ADC_Channel_12
	#define ACV_SAMP_CoalMotorOverload ADC_Channel_13
	#define ACV_SAMP_FengmenCtrMod ADC_Channel_2
	#define ACV_SAMP_FengmenOverload ADC_Channel_1	//风门电流来判断运行状态


	 //工作模式定义
   #define  STOP_MODE    0   //手动模式，待机模式
   #define  RUN_MODE     1
   #define  ALARM_MODE   2
   #define  SET_MODE     3
	 #define  QUERY_MODE   4   //查询历史数据记录
	 #define  DEBUG_MODE   5   //查询历史数据记录
	 #define  DEBUG_MODEV  6   //查询历史数据记录
   #define  DEBUG_MasterAddressSet  7   //查询历史数据记录
   #define  DEBUG_SlaveAddressSet   8	 //查询历史数据记录
	
	

	//运行曲线定义
	#define  SELFSET     0
    #define  DWNCURV     1  //下部曲线
    #define  MDICURV     2  //中部曲线
	#define  UPPCURV     3  //上部曲线

	//设置索引
	#define  CURV_INDEX  0  //设置烘烤工艺、曲线
	#define  PARA_INDEX  1  //设置参数
  #define  UPDW_INDEX  2  //设置上、下棚主控
  #define  DORT_INDEX  3  //设置日期或时间

  //RUNLED	

	#define SET_RUNLED GPIOA->BSRR|= GPIO_Pin_9  //PA9
	#define CLR_RUNLED GPIOA->BRR |= GPIO_Pin_9  //PA9	
	#define REVERSE_RUNLED GPIO_WriteBit(GPIOA, GPIO_Pin_9, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_9)))

   #define REVERSE_RESETCPU GPIO_WriteBit(GPIOB, GPIO_Pin_15, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_15)))

	//SETLED
  #define CLR_SETLED		GPIOC->BRR |= GPIO_Pin_6//PC6
  #define SET_SETLED		GPIOC->BSRR|= GPIO_Pin_6//PC6
	//DS18B20
	#define SET_SDS18B20 GPIOB->BSRR |= GPIO_Pin_14
	#define CLR_SDS18B20 GPIOB->BRR |= GPIO_Pin_14

    //鼓风机
//	#define FAN_RUN  GPIOC->BSRR |= GPIO_Pin_13  //PC13
//	#define FAN_STOP GPIOC->BRR |= GPIO_Pin_13   //PC13	

	#define JIAMEIJI_FORWARD  {GPIOC->BSRR |= GPIO_Pin_13; GPIOC->BRR |= GPIO_Pin_10;}
	#define JIAMEIJI_BACK     {GPIOC->BSRR |= GPIO_Pin_13; GPIOC->BSRR |= GPIO_Pin_10;}
	#define JIAMEIJI_STOP     {GPIOC->BRR  |= GPIO_Pin_13; GPIOC->BRR |= GPIO_Pin_10;}
//		#define JIAMEIJI_FORWARD  {GPIOC->BSRR |= GPIO_Pin_13; GPIOC->BSRR |= GPIO_Pin_10;}
//		#define JIAMEIJI_STOP     {GPIOC->BRR  |= GPIO_Pin_13; GPIOC->BRR |= GPIO_Pin_10;}
 

   #define FAN_RUN  GPIOA->BSRR |= GPIO_Pin_8  //PA7
   #define FAN_STOP GPIOA->BRR |= GPIO_Pin_8 //PA7

	 //控制循环风机
   #define FENGJI_RUN  GPIOD->BSRR |= GPIO_Pin_2  //PC13  //PA7
   #define FENGJI_STOP GPIOD->BRR |= GPIO_Pin_2  //PC13

	 //风门电机
//   #define    WIND_OPEN     {GPIOA->BSRR |= GPIO_Pin_4; GPIOA->BSRR |= GPIO_Pin_3;}
//   #define    WIND_CLOS     {GPIOA->BRR |= GPIO_Pin_4; GPIOA->BSRR |= GPIO_Pin_3;}

	#define    WIND_OPEN     {GPIOA->BRR |= GPIO_Pin_4; GPIOB->BSRR |= GPIO_Pin_3;}
	#define    WIND_CLOS     {GPIOA->BSRR |= GPIO_Pin_4; GPIOB->BRR |= GPIO_Pin_3;}


	#define    WIND_STOP    {GPIOA->BRR |= GPIO_Pin_4;GPIOB->BRR |= GPIO_Pin_3;}

	#define    WIND_RUN_TIME   50
	#define    FENGJI_DELAY    8
	
	#define		uiAcCurVal_Gaodi	85
	#define		uiAcCurVal_Overload	75


#endif
