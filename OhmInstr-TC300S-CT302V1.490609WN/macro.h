#ifndef _MACRO_H_
#define _MACRO_H_ 1

    //���忾������
    #define  OVENTYPE     0      //0: ��������ʽ��1�������½�ʽ

    #define  TRUE         1
	#define  FALSE        0  

	#define     UPSENSOR        0x00      //�����½�ʽ����
	#define     DWSENSOR        0x01      //��������ʽ����

	//������ֵ��Ż���������
	#define  DNDRY       1  //0   //������� 
	#define  DNWET       2  //1   //����ʪ��
	#define  UPDRY       3  //2   //������� 
  #define  UPWET       0  //3   //����ʪ��

	#define ACV_SAMP_CH0 ADC_Channel_0
	#define ACC_SAMP_CH0 ADC_Channel_10
	#define ACC_SAMP_CH1 ADC_Channel_11
	#define ACC_SAMP_CH2 ADC_Channel_12
	#define ACV_SAMP_CoalMotorOverload ADC_Channel_13
	#define ACV_SAMP_FengmenCtrMod ADC_Channel_2
	#define ACV_SAMP_FengmenOverload ADC_Channel_1	//���ŵ������ж�����״̬


	 //����ģʽ����
   #define  STOP_MODE    0   //�ֶ�ģʽ������ģʽ
   #define  RUN_MODE     1
   #define  ALARM_MODE   2
   #define  SET_MODE     3
	 #define  QUERY_MODE   4   //��ѯ��ʷ���ݼ�¼
	 #define  DEBUG_MODE   5   //��ѯ��ʷ���ݼ�¼
	 #define  DEBUG_MODEV  6   //��ѯ��ʷ���ݼ�¼
   #define  DEBUG_MasterAddressSet  7   //��ѯ��ʷ���ݼ�¼
   #define  DEBUG_SlaveAddressSet   8	 //��ѯ��ʷ���ݼ�¼
	
	

	//�������߶���
	#define  SELFSET     0
    #define  DWNCURV     1  //�²�����
    #define  MDICURV     2  //�в�����
	#define  UPPCURV     3  //�ϲ�����

	//��������
	#define  CURV_INDEX  0  //���ú濾���ա�����
	#define  PARA_INDEX  1  //���ò���
  #define  UPDW_INDEX  2  //�����ϡ���������
  #define  DORT_INDEX  3  //�������ڻ�ʱ��

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

    //�ķ��
//	#define FAN_RUN  GPIOC->BSRR |= GPIO_Pin_13  //PC13
//	#define FAN_STOP GPIOC->BRR |= GPIO_Pin_13   //PC13	

	#define JIAMEIJI_FORWARD  {GPIOC->BSRR |= GPIO_Pin_13; GPIOC->BRR |= GPIO_Pin_10;}
	#define JIAMEIJI_BACK     {GPIOC->BSRR |= GPIO_Pin_13; GPIOC->BSRR |= GPIO_Pin_10;}
	#define JIAMEIJI_STOP     {GPIOC->BRR  |= GPIO_Pin_13; GPIOC->BRR |= GPIO_Pin_10;}
//		#define JIAMEIJI_FORWARD  {GPIOC->BSRR |= GPIO_Pin_13; GPIOC->BSRR |= GPIO_Pin_10;}
//		#define JIAMEIJI_STOP     {GPIOC->BRR  |= GPIO_Pin_13; GPIOC->BRR |= GPIO_Pin_10;}
 

   #define FAN_RUN  GPIOA->BSRR |= GPIO_Pin_8  //PA7
   #define FAN_STOP GPIOA->BRR |= GPIO_Pin_8 //PA7

	 //����ѭ�����
   #define FENGJI_RUN  GPIOD->BSRR |= GPIO_Pin_2  //PC13  //PA7
   #define FENGJI_STOP GPIOD->BRR |= GPIO_Pin_2  //PC13

	 //���ŵ��
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
