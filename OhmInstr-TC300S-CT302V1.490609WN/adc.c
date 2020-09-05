#include "stm32f10x_lib.h"
#include "system.h"

unsigned int i32SampSum = 0;

#define VOLSCALE     31  //  DA   	  ??????  ???100?
						 //3.3/4096 / 510*150K   /1.414/2

#define CURSCALE     14  //  DA   	  采样电阻比例  放大了100倍
						 //3.3/4096 / 510*150K   /1.414/2

void ADCInit(void)
{
  ADC_InitTypeDef ADC_InitStructure;

  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel14 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_41Cycles5);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
     
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

// 获取片内AD的转换结果，计算电池电压（电量）
unsigned int uiGaugeACV(unsigned char ch) 
{
  int i;
  u32 adc_result;
  unsigned int reval;

  ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_41Cycles5);	//更改为ADC13:电压采样
  _delay_us(800);
  i32SampSum = 0;
  for(i=0;i<400;i++)
  {
    ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	while(RESET==ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));	//等待转换完成
		adc_result=ADC1->DR;
	i32SampSum  = i32SampSum + adc_result;
  	ADC_ClearFlag(ADC1,ADC_FLAG_EOC);					//?????
	_delay_us(48);
  }
   reval = i32SampSum * 0.0045;
   i32SampSum = 0;
  if(reval<=30) reval=0;
   return reval;
}
unsigned int uiGaugeACC(unsigned char ch)
{  
  int i;
  u32 adc_result,reval;
  u32 adc_max=0;
  reval =0;
  adc_result = 0;

  ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_41Cycles5);
  _delay_us(800);
  for(i=0;i<400;i++)
  {
	while(RESET==ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));	//等待转换完成
		adc_result=ADC1->DR;
  	ADC_ClearFlag(ADC1,ADC_FLAG_EOC);					//清除标志位
	if(adc_result>adc_max)
	   adc_max=adc_result;
	_delay_us(40);
  }
  reval =adc_max/6;
  if(reval<=10)reval=0;//零漂	                                                                                                                  
  return reval;
}

